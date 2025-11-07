#!/usr/bin/env bash
set -eEuo pipefail

trap 'echo "[ERROR] at line $LINENO"; exit 2' ERR

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

SHELL_BIN="${SHELL_BIN:-$ROOT_DIR/minishell}"
BASH_BIN="${BASH_BIN:-/bin/bash}"
TIMEOUT_BIN="${TIMEOUT_BIN:-$(command -v timeout || command -v gtimeout || true)}"
PROMPT_REGEX="${PROMPT_REGEX:-^minishell[$#]? }"

TMPDIR_ROOT="$ROOT_DIR/tests/.tmp"
WORKDIR="$TMPDIR_ROOT/work"
LOGDIR="$TMPDIR_ROOT/logs"

ensure_dir() { mkdir -p "$1"; }
run_to() { if [[ -n "${TIMEOUT_BIN:-}" ]]; then "$TIMEOUT_BIN" "$@"; else "$@"; fi; }

c_green="\033[32m"; c_red="\033[31m"; c_yellow="\033[33m"; c_blue="\033[34m"; c_dim="\033[2m"; c_reset="\033[0m"
ok=0; ng=0; sk=0

reset_sandbox() {
  rm -rf "$WORKDIR"
  ensure_dir "$WORKDIR"
  printf "line1\nline2 \$USER\n" > "$WORKDIR/infile"
  : > "$WORKDIR/outfile"
  printf "not-exec\n" > "$WORKDIR/noexec.sh"
  chmod 644 "$WORKDIR/noexec.sh"
  mkdir -p "$WORKDIR/outdir"
  rm -f "$WORKDIR/loopA" "$WORKDIR/loopB" 2>/dev/null || true
  ln -sfn loopB "$WORKDIR/loopA" 2>/dev/null || true
  ln -sfn loopA "$WORKDIR/loopB" 2>/dev/null || true
}

run_shell_pair() {
  local INPUT="$1"
  local TNAME="$2"

  ensure_dir "$LOGDIR"; ensure_dir "$WORKDIR"

  local in_file="$LOGDIR/$TNAME.input"
  local ms_raw="$LOGDIR/$TNAME.ms.raw"
  local ms_err="$LOGDIR/$TNAME.ms.err"
  local sh_out="$LOGDIR/$TNAME.sh.out"
  local sh_err="$LOGDIR/$TNAME.sh.err"

  {
    printf "%s\n" "$INPUT"
    printf "echo __STATUS:$?\n"
    printf "exit\n"
  } > "$in_file"

# minishell 側
reset_sandbox
( cd "$WORKDIR"
  ms_rc=0
  run_to 8s env -i PATH=/usr/bin:/bin:/usr/sbin:/sbin HOME="$WORKDIR" TERM=xterm \
    "$SHELL_BIN" < "$in_file" > "$ms_raw" 2> "$ms_err" || ms_rc=$?
  printf "%s" "$ms_rc" > "$LOGDIR/$TNAME.ms.rc"
)

# bash 側
reset_sandbox
( cd "$WORKDIR"
  sh_rc=0
  run_to 5s env -i PATH=/usr/bin:/bin:/usr/sbin:/sbin HOME="$WORKDIR" TERM=xterm \
    "$BASH_BIN" --noprofile --norc < "$in_file" > "$sh_out" 2> "$sh_err" || sh_rc=$?
  printf "%s" "$sh_rc" > "$LOGDIR/$TNAME.sh.rc"
)

sed -E $'s/\x1B\\[[0-9;]*[A-Za-z]//g' "$ms_raw" \
  | sed -E 's/[\x00-\x08\x0B\x0C\x0E-\x1F]//g' \
  | sed -E 's/[[:space:]]*minishell[$#]?[[:space:]].*$//' \
  | sed -E '/^[[:space:]]*(minishell[$#]?|heredoc>)([[:space:]]|$)/d' \
  | sed -E 's/[[:space:]]+$//' \
  | tr -d '\r' \
  > "$LOGDIR/$TNAME.ms.norm"
  sed -E 's/[[:space:]]+$//' "$sh_out" | tr -d '\r' > "$LOGDIR/$TNAME.sh.norm"

  local ms_status sh_status
  ms_status="$(grep -oE '__STATUS:[0-9]+' "$LOGDIR/$TNAME.ms.norm" | tail -n1 | cut -d: -f2 || true)"
  sh_status="$(grep -oE '__STATUS:[0-9]+' "$LOGDIR/$TNAME.sh.norm" | tail -n1 | cut -d: -f2 || true)"
if [[ -z "${ms_status:-}" ]]; then ms_status="$(cat "$LOGDIR/$TNAME.ms.rc" 2>/dev/null || echo 255)"; fi
if [[ -z "${sh_status:-}" ]]; then sh_status="$(cat "$LOGDIR/$TNAME.sh.rc" 2>/dev/null || echo 255)"; fi


  grep -v '^__STATUS:' "$LOGDIR/$TNAME.ms.norm" > "$LOGDIR/$TNAME.ms.body" || true
  grep -v '^__STATUS:' "$LOGDIR/$TNAME.sh.norm" > "$LOGDIR/$TNAME.sh.body" || true

  printf "%s" "$ms_status" > "$LOGDIR/$TNAME.ms.status"
  printf "%s" "$sh_status" > "$LOGDIR/$TNAME.sh.status"
}

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
  # minishell 実行（RCフォールバック用に終了コードを保存）
  local ms_rc=0
  run_to 5s env -i PATH=/usr/bin:/bin:/usr/sbin:/sbin HOME="$WORKDIR" TERM=xterm \
    "$SHELL_BIN" < "$in_file" > "$LOGDIR/$name.ms.out" 2> "$LOGDIR/$name.ms.err" || ms_rc=$?
  printf "%s" "$ms_rc" > "$LOGDIR/$name.ms.rc"

  # ---- minishell 出力の正規化（色/制御/プロンプト/PS2/行中プロンプトの除去）----
  # PROMPT_REGEX は従来のものも尊重しつつ、より強いフィルタを併用
sed -E $'s/\x1B\\[[0-9;]*[A-Za-z]//g' "$ms_raw" \
  | sed -E "/$PROMPT_REGEX/d" \
  | sed -E 's/[\x00-\x08\x0B\x0C\x0E-\x1F]//g' \
  | sed -E 's/[[:space:]]*minishell[$#]?[[:space:]].*$//' \
  | sed -E '/^[[:space:]]*(minishell[$#]?|heredoc>)([[:space:]]|$)/d' \
  | sed -E 's/[[:space:]]+$//' \
  | tr -d '\r' \
  > "$LOGDIR/$TNAME.ms.norm"

  # ---- ステータス抽出（__STATUS が無ければ RC を採用）----
  local ms_status
  ms_status="$(grep -oE '__STATUS:[0-9]+' "$LOGDIR/$name.ms.norm" | tail -n1 | cut -d: -f2 || true)"
  if [[ -z "${ms_status:-}" ]]; then
    ms_status="$(cat "$LOGDIR/$name.ms.rc" 2>/dev/null || echo 255)"
  fi

  # ---- 本文（__STATUS 行を除去してマッチング用に）----
  grep -v '^__STATUS:' "$LOGDIR/$name.ms.norm" > "$LOGDIR/$name.ms.body" || true

  # ---- マッチ判定（空出力も '^$' / '^(exit)?$' なら合格扱い）----
  local matched=1
  if [[ -s "$LOGDIR/$name.ms.body" ]]; then
    if grep -E "$regex" "$LOGDIR/$name.ms.body" >/dev/null 2>&1; then matched=0; fi
  else
    if [[ "$regex" == '^$' || "$regex" == '^(exit)?$' ]]; then matched=0; fi
  fi

  if [[ $matched -eq 0 ]] && { [[ -z "$expect_status" ]] || [[ "$ms_status" == "$expect_status" ]]; }; then
    printf "${c_green}[OK]${c_reset} %s\n" "$desc"; ok=$((ok+1))
  else
    printf "${c_red}[NG]${c_reset} %s\n" "$desc"
    printf "${c_dim}-- minishell status: %s --${c_reset}\n" "$ms_status"
    printf "${c_yellow}-- output --${c_reset}\n"; sed -n '1,200p' "$LOGDIR/$name.ms.body" || true
    ng=$((ng+1))
  fi
}

main() {
  echo -e "${c_blue}== minishell test runner (Mandatory only, no ;) ==${c_reset}"

  ensure_dir "$TMPDIR_ROOT"; ensure_dir "$WORKDIR"; ensure_dir "$LOGDIR"
  [[ -x "$SHELL_BIN" ]] || { echo -e "${c_red}minishell がありません:${c_reset} $SHELL_BIN"; exit 2; }
  [[ -x "$BASH_BIN"   ]] || { echo -e "${c_red}bash がありません:${c_reset} $BASH_BIN"; exit 2; }

  # 1) 基本
  case_eq_all "echo 単純"                  $'echo hello'                                   "basic_echo_1"
  case_eq_all "echo -n 複数"               $'echo -n -n hi world'                         "basic_echo_2"
  case_eq_all "pwd"                        $'pwd'                                         "basic_pwd"
  case_eq_all "env（環境あり）"            $'export AAA=bbb\nenv | grep ^AAA='            "basic_env"

  # 2) 引用符・展開
  case_eq_all "SQは展開しない"             $'echo \x27$USER\x27'                          "q_single_noexpand"
  case_eq_all "DQは展開する"               $'echo "$USER"'                                "q_double_expand"
  case_eq_all "混在結合"                   $'echo a"$USER"b\x27c\x27'                     "q_mix_concat"
  case_eq_all "直前ステータス"             $'false\necho $?\ntrue\necho $?'               "q_status_chain"

  # 3) コマンド検索/エラー
  case_eq_status "not found"               $'nosuchcmd'                                   "err_not_found"
  case_eq_status "ディレクトリ実行"        $'./'                                          "err_dir_exec"
  case_eq_status "実行権なし"              $'./noexec.sh'                                 "err_noexec"
  case_eq_status "実行形式エラー"          $'printf not-a-binary > a.out\nchmod +x a.out\n./a.out' "err_exec_fmt"

  # 4) リダイレクト
  case_file_eq "出力 >"                    $'echo A > outfile'                            "redir_out_trunc" "outfile" "A\n"
  case_file_eq "追記 >>"                   $'echo A > outfile\necho B >> outfile'         "redir_out_app"   "outfile" "A\nB\n"
  case_eq_all  "入力 < とパイプ"           $'cat < infile | grep line2'                   "redir_in_pipe"
  case_eq_status "ディレクトリへ > 失敗"   $'echo a > outdir'                             "redir_dir_fail"
  case_eq_status "未完 > は構文エラー"     $'echo x >'                                    "redir_incomplete"

  # 5) パイプ
  case_eq_all  "単純パイプ"  'printf "a\\nb\\nc\\n" | grep b'              "pipe_simple"
  case_eq_all  "多段パイプ"  'printf "a\\nb\\nc\\n" | grep b | tr a-z A-Z'  "pipe_multi"
  case_eq_all  "パイプ＋リダイレクト"      $'echo one | cat | cat > outfile\ncat outfile' "pipe_redir"
  case_eq_all  "右端の終了コード"          $'true | false\necho $?'                       "pipe_right_status"
  case_eq_all  "左失敗でも右端準拠"        $'nosuch | cat\necho $?'                       "pipe_left_fail"

  # 6) ヒアドキュメント
  case_eq_all "heredoc 展開あり"           $'cat <<EOF\nuser:$USER\nEOF'                  "hdoc_expand"
  case_eq_all "heredoc 展開なし(引用)"     $'cat <<\x27EOF\x27\nuser:$USER\nEOF'          "hdoc_no_expand"

  # 7) export/unset（Mandatoryのみ）
  case_eq_all    "export→env 伝播"         $'unset FOO 2>/dev/null || :\nexport FOO=bar\nenv | grep ^FOO=' "export_env"
  case_eq_status "export 無効ID"           $'export 1BAD=V'                               "export_invalid"
  case_eq_all    "unset で消える"          $'export TMPX=1\nunset TMPX\necho "$TMPX"'     "unset_remove"

  # 8) exit（空出力も許容）
  case_match_ms "exit 単独"                $'exit'                                        "exit_plain" '^(exit)?$' ""
  case_match_ms "exit 数値"                $'exit 42'                                     "exit_num"   '^(exit)?$' ""
  case_eq_status "exit 非数"               $'exit notnum'                                 "exit_notnum"
  case_eq_status "exit 引数過多(終了しない)" $'exit 1 2'                                  "exit_many"

  # 9) 解析エラー
  case_eq_status "未閉DQ"                  $'echo "abc'                                   "syn_unclosed_dq"
  case_eq_status "未閉SQ"                  "echo 'abc"                                    "syn_unclosed_sq"
  case_eq_status "孤立パイプ"              $'|'                                           "syn_lone_pipe"
  case_eq_status "オペランド欠落(<)"       $'cat <'                                       "syn_missing_op"
  case_eq_status "<<< は構文エラー扱い"    $'<<< a'                                       "syn_bad_op"

  # 10) ストレス
  longword="$(printf 'x%.0s' {1..50000})"
  case_match_ms "超長トークン(落ちない)"   "echo $longword"                               "stress_longword" "^" ""

  # 11) 複数行スクリプト
  case_eq_all "複数行"                     $'echo a\necho b\nfalse\necho $?'              "script_multi"

  echo -e "${c_blue}== Summary ==${c_reset} OK:${ok}  NG:${ng}  SKIP:${sk}"
  [[ $ng -eq 0 ]]
}

main "$@"
