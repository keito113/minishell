#!/usr/bin/env bash
set -eEuo pipefail

# ---- 失敗時に行番号を出すトラップ ----
trap 'echo "[ERROR] at line $LINENO"; exit 2' ERR

# ---- どこから実行しても動くように絶対パス化 ----
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

SHELL_BIN="${SHELL_BIN:-$ROOT_DIR/minishell}"
BASH_BIN="${BASH_BIN:-/bin/bash}"
TIMEOUT_BIN="${TIMEOUT_BIN:-$(command -v timeout || command -v gtimeout || true)}"
PROMPT_REGEX="${PROMPT_REGEX:-^minishell[$#]? }"

TMPDIR_ROOT="$ROOT_DIR/tests/.tmp"
WORKDIR="$TMPDIR_ROOT/work"
LOGDIR="$TMPDIR_ROOT/logs"

# ---- 便利関数 ----
ensure_dir() { mkdir -p "$1"; }

run_to() {
  if [[ -n "${TIMEOUT_BIN:-}" ]]; then
    "$TIMEOUT_BIN" "$@"
  else
    "$@"
  fi
}

# 色
c_green="\033[32m"; c_red="\033[31m"; c_yellow="\033[33m"; c_blue="\033[34m"; c_dim="\033[2m"; c_reset="\033[0m"

ok=0; ng=0; sk=0

# ---- サンドボックスの初期化 ----
reset_sandbox() {
  rm -rf "$WORKDIR"
  ensure_dir "$WORKDIR"
  printf "line1\nline2 \$USER\n" > "$WORKDIR/infile"
  : > "$WORKDIR/outfile"
  printf "not-exec\n" > "$WORKDIR/noexec.sh"
  chmod 644 "$WORKDIR/noexec.sh"
  mkdir -p "$WORKDIR/dir_as_cmd"
  # 相互リンク（失敗は無視）
  rm -f "$WORKDIR/loopA" "$WORKDIR/loopB" 2>/dev/null || true
  ln -sfn loopB "$WORKDIR/loopA" 2>/dev/null || true
  ln -sfn loopA "$WORKDIR/loopB" 2>/dev/null || true
}

# ---- minishell と bash を同条件で実行 ----
run_shell_pair() {
  local INPUT="$1"    # 複数行OK（ここに実行したいコマンドを入れる）
  local TNAME="$2"    # テスト名（ログファイル名に使う）

  ensure_dir "$LOGDIR"
  ensure_dir "$WORKDIR"

  # ログ/一時ファイルのパスを定義
  local in_file="$LOGDIR/$TNAME.input"
  local ms_raw="$LOGDIR/$TNAME.ms.raw"     # minishell の“生”出力
  local ms_err="$LOGDIR/$TNAME.ms.err"
  local sh_out="$LOGDIR/$TNAME.sh.out"     # bash の出力
  local sh_err="$LOGDIR/$TNAME.sh.err"

  # 実行する入力を作成：
  #  - 本文
  #  - 最後に「echo __STATUS:$?」で終了ステータスを吐く
  #  - 最後に exit で終了
  {
    printf "%s\n" "$INPUT"
    printf "echo __STATUS:$?\n"
    printf "exit\n"
  } > "$in_file"

  # ---- minishell 側の実行 ----
  # env -i …：環境変数を最小にして実行（差分が出にくい）
  # timeout（あれば）：ハング対策
  reset_sandbox
  ( cd "$WORKDIR"
    run_to 8s env -i PATH=/usr/bin:/bin:/usr/sbin:/sbin HOME="$WORKDIR" TERM=xterm \
      "$SHELL_BIN" < "$in_file" > "$ms_raw" 2> "$ms_err" || true
  )

  # ---- bash 側の実行 ----
  reset_sandbox
  ( cd "$WORKDIR"
    run_to 5s env -i PATH=/usr/bin:/bin:/usr/sbin:/sbin HOME="$WORKDIR" TERM=xterm \
      "$BASH_BIN" --noprofile --norc < "$in_file" > "$sh_out" 2> "$sh_err" || true
  )

  # ---- minishell 出力の正規化 ----
  # 1) ANSIエスケープ除去（色などの制御コード）
  # 2) 行頭が "minishell$" で始まる行（プロンプトと入力のエコー）を**丸ごと捨てる**
  # 3) 行末の余分な空白とCR(\r)を除去
  sed -E $'s/\x1B\\[[0-9;]*[A-Za-z]//g' "$ms_raw" \
    | sed -E '/^minishell[$#]?($| )/d' \
    | sed -E 's/[[:space:]]+$//' \
    | tr -d '\r' \
    > "$LOGDIR/$TNAME.ms.norm"

  # ---- bash 出力の正規化 ----
  sed -E 's/[[:space:]]+$//' "$sh_out" | tr -d '\r' > "$LOGDIR/$TNAME.sh.norm"

  # ---- 終了ステータスの抽出（見つからない場合でもエラーにしない）----
  local ms_status sh_status
  ms_status="$(grep -oE '__STATUS:[0-9]+' "$LOGDIR/$TNAME.ms.norm" | tail -n1 | cut -d: -f2 || true)"
  sh_status="$(grep -oE '__STATUS:[0-9]+' "$LOGDIR/$TNAME.sh.norm" | tail -n1 | cut -d: -f2 || true)"
  : "${ms_status:=255}"   # 変数が空なら 255 を入れる（デフォルト代入）
  : "${sh_status:=255}"

  # ステータス行を除いた“本文”を作る（ファイルが空でも失敗扱いにしない）
  grep -v '^__STATUS:' "$LOGDIR/$TNAME.ms.norm" > "$LOGDIR/$TNAME.ms.body" || true
  grep -v '^__STATUS:' "$LOGDIR/$TNAME.sh.norm" > "$LOGDIR/$TNAME.sh.body" || true

  # ステータスを保存（後続の比較関数が読む）
  printf "%s" "$ms_status" > "$LOGDIR/$TNAME.ms.status"
  printf "%s" "$sh_status" > "$LOGDIR/$TNAME.sh.status"
}


# ---- 判定ユーティリティ ----
case_eq_all() {
  local desc="$1" ; local script="$2" ; local name="$3"
  run_shell_pair "$script" "$name"
  local ms_status sh_status
  ms_status="$(cat "$LOGDIR/$name.ms.status")"
  sh_status="$(cat "$LOGDIR/$name.sh.status")"
  if diff -u "$LOGDIR/$name.sh.body" "$LOGDIR/$name.ms.body" >"$LOGDIR/$name.diff" 2>&1 && [[ "$ms_status" == "$sh_status" ]]; then
    printf "${c_green}[OK]${c_reset} %s\n" "$desc"; ok=$((ok+1))
  else
    printf "${c_red}[NG]${c_reset} %s\n" "$desc"
    printf "${c_dim}-- status: bash=%s, minishell=%s --${c_reset}\n" "$sh_status" "$ms_status"
    printf "${c_yellow}-- diff --${c_reset}\n"; sed -n '1,120p' "$LOGDIR/$name.diff" || true
    ng=$((ng+1))
  fi
}

case_eq_status() {
  local desc="$1" ; local script="$2" ; local name="$3"
  run_shell_pair "$script" "$name"
  local ms_status sh_status
  ms_status="$(cat "$LOGDIR/$name.ms.status")"
  sh_status="$(cat "$LOGDIR/$name.sh.status")"
  if [[ "$ms_status" == "$sh_status" ]]; then
    printf "${c_green}[OK]${c_reset} %s\n" "$desc"; ok=$((ok+1))
  else
    printf "${c_red}[NG]${c_reset} %s\n" "$desc"
    printf "${c_dim}-- status: bash=%s, minishell=%s --${c_reset}\n" "$sh_status" "$ms_status"
    ng=$((ng+1))
  fi
}

case_file_eq() {
  local desc="$1" ; local script="$2" ; local name="$3" ; local path_rel="$4" ; local expect_content="$5"
  ensure_dir "$LOGDIR"; ensure_dir "$WORKDIR"
  local in_file="$LOGDIR/$name.input"
  ensure_dir "$(dirname "$in_file")"
  {
    printf "%s\n" "$script"
    printf "exit\n"
  } > "$in_file"
  reset_sandbox
  run_to 5s env -i PATH=/usr/bin:/bin:/usr/sbin:/sbin HOME="$WORKDIR" TERM=xterm \
    "$SHELL_BIN" < "$in_file" > "$LOGDIR/$name.ms.out" 2> "$LOGDIR/$name.ms.err" || true
  local got; got="$(cat "$WORKDIR/$path_rel" 2>/dev/null || true)"
  if [[ "$got" == "$expect_content" ]]; then
    printf "${c_green}[OK]${c_reset} %s\n" "$desc"; ok=$((ok+1))
  else
    printf "${c_red}[NG]${c_reset} %s\n" "$desc"
    printf "${c_dim}-- file: %s --${c_reset}\n" "$path_rel"
    printf "expected:\n%s\n---\ngot:\n%s\n" "$expect_content" "$got"
    ng=$((ng+1))
  fi
}

case_match_ms() {
  local desc="$1" ; local script="$2" ; local name="$3" ; local regex="$4" ; local expect_status="${5:-}"
  ensure_dir "$LOGDIR"; ensure_dir "$WORKDIR"
  local in_file="$LOGDIR/$name.input"
  ensure_dir "$(dirname "$in_file")"
  {
    printf "%s\n" "$script"
    printf "echo __STATUS:$?\n"
    printf "exit\n"
  } > "$in_file"
  reset_sandbox
  run_to 5s env -i PATH=/usr/bin:/bin:/usr/sbin:/sbin HOME="$WORKDIR" TERM=xterm \
    "$SHELL_BIN" < "$in_file" > "$LOGDIR/$name.ms.out" 2> "$LOGDIR/$name.ms.err" || true
  sed -E "s/$PROMPT_REGEX//" "$LOGDIR/$name.ms.out" | sed -E 's/[[:space:]]+$//' | tr -d '\r' > "$LOGDIR/$name.ms.norm"
  local ms_status; ms_status="$(grep -oE '__STATUS:[0-9]+' "$LOGDIR/$name.ms.norm" | tail -n1 | cut -d: -f2 || echo 255)"
  if grep -E "$regex" "$LOGDIR/$name.ms.norm" >/dev/null 2>&1 \
     && { [[ -z "$expect_status" ]] || [[ "$ms_status" == "$expect_status" ]]; }; then
    printf "${c_green}[OK]${c_reset} %s\n" "$desc"; ok=$((ok+1))
  else
    printf "${c_red}[NG]${c_reset} %s\n" "$desc"
    printf "${c_dim}-- minishell status: %s --${c_reset}\n" "$ms_status"
    printf "${c_yellow}-- output --${c_reset}\n"; sed -n '1,120p' "$LOGDIR/$name.ms.norm" || true
    ng=$((ng+1))
  fi
}

main() {
  echo -e "${c_blue}== minishell test runner ==${c_reset}"

  # 事前に作成（ここで失敗すると早期に落ちる）
  ensure_dir "$TMPDIR_ROOT"; ensure_dir "$WORKDIR"; ensure_dir "$LOGDIR"

  [[ -x "$SHELL_BIN" ]] || { echo -e "${c_red}minishell がありません:${c_reset} $SHELL_BIN"; exit 2; }
  [[ -x "$BASH_BIN"   ]] || { echo -e "${c_red}bash がありません:${c_reset} $BASH_BIN"; exit 2; }

  # 1) 基本
  case_eq_all "echo 単純"                  $'echo hello'                                   "basic_echo_1"
  case_eq_all "echo -n 複数"               $'echo -n -n hi world'                         "basic_echo_2"
  case_eq_all "pwd"                        $'pwd'                                         "basic_pwd"
  case_eq_all "env（環境あり）"            $'export AAA=bbb\nenv | grep ^AAA='            "basic_env"

  # 2) 引用符・展開
  case_eq_all "SQは展開しない"             $'echo '\'$USER\'''                            "q_single_noexpand"
  case_eq_all "DQは展開する"               $'echo "$USER"'                               "q_double_expand"
  case_eq_all "混在結合"                   $'echo a"$USER"b'\''c'                         "q_mix_concat"
  case_eq_all "直前ステータス"             $'false\necho $?; true\necho $?'               "q_status_chain"

  # 3) コマンド検索/エラー
  case_eq_status "not found"               $'nosuchcmd'                                   "err_not_found"
  case_eq_status "ディレクトリ実行"        $'./'                                         "err_dir_exec"
  case_eq_status "実行権なし"              $'./noexec.sh'                                 "err_noexec"

  # 4) リダイレクト
  case_file_eq "出力 >"                    $'echo A > outfile'                            "redir_out_trunc" "outfile" "A\n"
  case_file_eq "追記 >>"                   $'echo A > outfile\necho B >> outfile'         "redir_out_app"   "outfile" "A\nB\n"
  case_eq_all  "入力 < とパイプ"           $'cat < infile | grep line2'                   "redir_in_pipe"
  case_eq_status "不正FD左辺(依存)"        $'999999999999>out'                            "redir_bad_fd"

  # 5) パイプ
  case_eq_all  "単純パイプ"                $'printf "a\nb\nc\n" | grep b'                 "pipe_simple"
  case_eq_all  "多段パイプ"                $'printf "a\nb\nc\n" | grep b | tr a-z A-Z'    "pipe_multi"
  case_eq_all  "パイプ＋リダイレクト"      $'echo one | cat | cat > outfile\ncat outfile' "pipe_redir"

  # 6) ヒアドキュメント
  case_eq_all "heredoc 展開あり"           $'cat <<EOF\nuser:$USER\nEOF'                  "hdoc_expand"
  case_eq_all "heredoc 展開なし(引用)"     $'cat <<'\''EOF'\''\nuser:$USER\nEOF'          "hdoc_no_expand"

  # 7) export/unset
  case_eq_all "export→env"                 $'export FOO=bar\nenv | grep ^FOO='            "export_env"
  case_eq_all "export +="                  $'export FOO=bar\nexport FOO+=baz\necho $FOO'  "export_append"
  case_eq_status "export 無効ID"           $'export 1BAD=V'                               "export_invalid"
  case_eq_all "unset で消える"             $'export TMPX=1\nunset TMPX\necho "$TMPX"'     "unset_remove"

  # 8) exit
  case_match_ms "exit 単独"                $'exit'                                        "exit_plain" '^$' ""
  case_match_ms "exit 数値"                $'exit 42'                                     "exit_num" '^$' ""
  case_eq_status "exit 非数"               $'exit notnum'                                 "exit_notnum"
  case_eq_status "exit 引数過多"           $'exit 1 2'                                    "exit_many"

  # 9) 解析エラー
  case_eq_status "未閉DQ"                  $'echo "abc'                                   "syn_unclosed_dq"
  case_eq_status "未閉SQ"                  $"echo 'abc"                                   "syn_unclosed_sq"
  case_eq_status "孤立パイプ"              $'|'                                           "syn_lone_pipe"
  case_eq_status "変な演算子"              $'<<< a'                                       "syn_bad_op"
  case_eq_status "オペランド欠落"          $'cat <'                                       "syn_missing_op"

  # 10) ストレス
  longword="$(printf 'x%.0s' {1..50000})"
  case_match_ms "超長トークン(落ちない)"   "echo $longword"                               "stress_longword" "^" ""

  # 11) 複数行スクリプト
  case_eq_all "複数行"                     $'echo a\necho b\nfalse\necho $?'              "script_multi"

  echo -e "${c_blue}== Summary ==${c_reset} OK:${ok}  NG:${ng}  SKIP:${sk}"
  [[ $ng -eq 0 ]]
}

main "$@"
