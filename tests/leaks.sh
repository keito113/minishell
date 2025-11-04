#!/usr/bin/env bash
set -euo pipefail

# ===== パス解決（どこから実行してもOK） =====
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SHELL_BIN="${SHELL_BIN:-$ROOT_DIR/minishell}"
VG_BIN="${VG_BIN:-$(command -v valgrind || true)}"
TIMEOUT_BIN="${TIMEOUT_BIN:-$(command -v timeout || command -v gtimeout || true)}"

if [[ -z "${VG_BIN}" ]]; then
  echo "valgrind が見つかりません。Linux で sudo apt-get install valgrind などで導入してください。" >&2
  exit 2
fi

TMP="$ROOT_DIR/tests/.leaks_tmp"
WORK="$TMP/work"
LOG="$TMP/logs"
SUPP="$ROOT_DIR/tests/valgrind.supp"
mkdir -p "$WORK" "$LOG"

run_to() { if [[ -n "${TIMEOUT_BIN:-}" ]]; then "$TIMEOUT_BIN" "$@"; else "$@"; fi; }

reset_sandbox() {
  rm -rf "$WORK"; mkdir -p "$WORK"
  printf "line1\nline2 \$USER\n" > "$WORK/infile"
  : > "$WORK/outfile"
  printf "not-exec\n" > "$WORK/noexec.sh"; chmod 644 "$WORK/noexec.sh"
  mkdir -p "$WORK/dir_as_cmd"
}

# =============== Valgrind 実行ヘルパ ===============
# 入力文字列を minishell に流して終了。Valgrind のログを解析し、readline 由来を除いた
# definitely/indirectly/possibly lost > 0 なら NG にする。
run_case_leaks() {
  local desc="$1" code="$2" name="$3"
  reset_sandbox
  local in="$LOG/$name.in" vglog="$LOG/$name.vg.log" out="$LOG/$name.out" err="$LOG/$name.err"

  {
    printf "%s\n" "$code"
    printf "exit\n"
  } > "$in"

  # trace-children=no: 子プロセス(外部コマンド)は追わない＝あなたの親プロセスだけ監査
  # error-exitcode=99: リークがあったら終了コード99に（ただし grep 解析も行う）
  run_to 15s env -i PATH=/usr/bin:/bin:/usr/sbin:/sbin HOME="$WORK" TERM=xterm \
    "$VG_BIN" --quiet --leak-check=full --show-leak-kinds=all \
    --track-origins=yes --trace-children=no --child-silent-after-fork=yes \
    --error-exitcode=99 --suppressions="$SUPP" --log-file="$vglog" \
    "$SHELL_BIN" < "$in" > "$out" 2> "$err" || true

  # 解析
  # 例: "definitely lost: 0 bytes in 0 blocks"
  dl=$(grep -Eo 'definitely lost: *[0-9,]+' "$vglog" | awk '{print $3}' | tr -d ',')
  il=$(grep -Eo 'indirectly lost: *[0-9,]+'  "$vglog" | awk '{print $3}' | tr -d ',')
  pl=$(grep -Eo 'possibly lost: *[0-9,]+'    "$vglog" | awk '{print $3}' | tr -d ',')

  # FDリーク検出（--track-fds=yes を併用したい場合は Valgrindをビルドオプションで）
  # ここでは一般的な "Open file descriptor" を目印にする
  fdleaks=$(grep -c 'Open file descriptor' "$vglog" || true)

  : "${dl:=0}"; : "${il:=0}"; : "${pl:=0}"; : "${fdleaks:=0}"

  if (( dl>0 || il>0 || pl>0 || fdleaks>0 )); then
    printf "\033[31m[LEAK]\033[0m %s  (def=%s, ind=%s, pos=%s, fd=%s)  log: %s\n" \
      "$desc" "$dl" "$il" "$pl" "$fdleaks" "$vglog"
    return 1
  else
    printf "\033[32m[OK]\033[0m    %s\n" "$desc"
    return 0
  fi
}

# ================= テストケース群 =================
ok=0; ng=0

case_leaks() { run_case_leaks "$@" && ok=$((ok+1)) || ng=$((ng+1)); }

main() {
  echo "== minishell leak suite (Linux/Valgrind) =="

  # 1) トークン/パースのみ（実行無し） → 直後に exit
  case_leaks "tokens: 空白/引用/連結" \
    $'echo a"$USER"b'\''c\n' \
    "tok_concat"

  case_leaks "syntax: 未閉クォート" \
    $'echo "abc\n' \
    "syn_unclosed_quote"

  case_leaks "syntax: 孤立パイプ" \
    $'|\n' \
    "syn_lone_pipe"

  # 2) expand（未定義/定義/数値展開）
  case_leaks "expand: 未定義は空" \
    $'unset X\n echo "$X"__END\n' \
    "exp_undef"

  case_leaks "expand: $? 連続参照" \
    $'false\n echo $?; true; echo $?\n' \
    "exp_status"

  # 3) env テーブル操作（生成/上書き/+=/無効ID/削除）
  case_leaks "env: export/set/append/unset" \
    $'export A=1\n export A+=2\n export B=\n unset A\n unset B\n' \
    "env_ops"

  case_leaks "env: 無効ID群（エラー経路）" \
    $'export 1BAD=V\n export =oops\n export +PLUS=1\n' \
    "env_invalid"

  # 4) リダイレクト 正常/エラー（オープン失敗経路の後始末）
  case_leaks "redir: > 追記と上書き" \
    $'echo A > outfile\n echo B >> outfile\n' \
    "redir_out"

  case_leaks "redir: 入力<nofile（失敗）" \
    $'cat < nofile\n' \
    "redir_in_missing"

  # 5) heredoc（展開あり/なし）— 終了後のバッファ/FDの解放
  case_leaks "heredoc: 展開あり" \
    $'cat <<EOF\nv:$USER\nEOF\n' \
    "hdoc_expand"

  case_leaks "heredoc: 展開なし(quoted)" \
    $'cat <<'\''EOF'\''\n$USER\nEOF\n' \
    "hdoc_noexpand"

  # 6) pipeline（ビルトイン/外部混在）— pipe/dup/close 経路
  case_leaks "pipe: 多段+redir（外部混在）" \
    $'printf "a\nb\nc\n" | grep b | tr a-z A-Z > outfile\n cat outfile\n' \
    "pipe_multi"

  # 7) 外部コマンドの失敗（cmd not found / ディレクトリ実行）
  case_leaks "exec: not found" \
    $'nosuchcommand\n' \
    "exec_notfound"

  case_leaks "exec: ディレクトリ実行（失敗）" \
    $'./\n' \
    "exec_dir"

  # 8) ストレス（長語/多トークン）
  longword="$(printf 'x%.0s' {1..50000})"
  case_leaks "stress: 超長トークン" \
    "echo $longword" \
    "stress_long"

  echo "== summary: OK=$ok, NG=$ng =="
  [[ $ng -eq 0 ]]
}

main "$@"
