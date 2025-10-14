#include "minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// static char	**make_argv(int n, const char *a0, const char *a1,
// const char *a2,
// 		const char *a3, const char *a4)
// {
// 	char	**v;

// 	(void)n; // 必要なら可変長に
// 	v = calloc(6, sizeof(char *));
// 	if (!v)
// 		return (NULL);
// 	v[0] = a0 ? strdup(a0) : NULL;
// 	v[1] = a1 ? strdup(a1) : NULL;
// 	v[2] = a2 ? strdup(a2) : NULL;
// 	v[3] = a3 ? strdup(a3) : NULL;
// 	v[4] = a4 ? strdup(a4) : NULL;
// 	v[5] = NULL;
// 	return (v);
// }

// // ---- redir リスト操作 -------------------------------------------------

// // 末尾追加（順序保持 = 後勝ちの検証に都合が良い）
// static t_redir	*push_redir_ex(t_redir **head, t_rtype kind,
// const char *arg,
// 		int fd_target)
// {
// 	t_redir	*r;
// 	t_redir	*t;

// 	r = calloc(1, sizeof(*r));
// 	if (!r)
// 		return (NULL);
// 	// まずゼロ化してから個別に設定（ゴミ値防止）
// 	// calloc なので0初期化済みだが、方針としてここにコメントを残す
// 	r->kind = kind;
// 	r->arg = arg ? strdup(arg) : strdup("");
// 	r->quoted = 0;
// 	r->hdoc_fd = -1;                            // heredocは別途セット
// 	r->fd_target = (fd_target >= 0) ? fd_target // 左辺FDを明示
// 									: -1;       // 未指定は -1（デフォルト: 入力=0/出力=1）
// 	r->next = NULL;
// 	if (!*head)
// 	{
// 		*head = r;
// 	}
// 	else
// 	{
// 		t = *head;
// 		while (t->next)
// 			t = t->next;
// 		t->next = r;
// 	}
// 	return (r);
// }

// // 左辺未指定版（糖衣）
// static inline t_redir	*push_redir(t_redir **head, t_rtype kind,
// 		const char *arg)
// {
// 	return (push_redir_ex(head, kind, arg, -1));
// }

// static t_redir	*push_redir_hdoc(t_redir **head, const char *limiter,
// 		int quoted)
// {
// 	t_redir	*r;
// 	t_redir	*t;

// 	r = calloc(1, sizeof(*r));
// 	if (!r)
// 		return (NULL);
// 	r->kind = R_HDOC;
// 	r->arg = limiter ? strdup(limiter) : strdup("");
// 	r->quoted = quoted;
// 	r->fd_target = -1; // 省略→STDIN
// 	r->hdoc_fd = -1;   // prepare_heredocs_for_cmd で埋まる
// 	r->next = NULL;
// 	if (!*head)
// 		*head = r;
// 	else
// 	{
// 		t = *head;
// 		while (t->next)
// 			t = t->next;
// 		t->next = r;
// 	}
// 	return (r);
// }

// // fd_target を明示する版（例: 0<<LIM）
// static t_redir	*push_redir_hdoc_ex(t_redir **head, const char *limiter,
// 		int quoted, int fd_target)
// {
// 	t_redir	*r;

// 	r = push_redir_hdoc(head, limiter, quoted);
// 	if (r)
// 		r->fd_target = fd_target;
// 	return (r);
// }

// static void	free_redirs(t_redir *r)
// {
// 	t_redir	*n;

// 	while (r)
// 	{
// 		n = r->next;
// 		free(r->arg);
// 		// r->hdoc_fd は収集側のポリシーに合わせて閉じる/閉じないを決める
// 		free(r);
// 		r = n;
// 	}
// }

// static void	free_argv(char **v)
// {
// 	if (!v)
// 		return ;
// 	for (int i = 0; v[i]; i++)
// 		free(v[i]);
// 	free(v);
// }

// static void	free_ast(t_ast *node)
// {
// 	if (!node)
// 		return ;
// 	if (node->type == AST_PIPE)
// 	{
// 		free_ast(node->as.pipe.left);
// 		free_ast(node->as.pipe.right);
// 		free(node);
// 	}
// 	else // AST_CMD
// 	{
// 		free_argv(node->as.cmd.argv);
// 		free_redirs(node->as.cmd.redirs);
// 		free(node);
// 	}
// }

// // ---- テストケース -----------------------------------------------------

// // // 1) grep b < infile > outfile
// // static void	build_case1_grep(t_ast *out_ast)
// // {
// // 	memset(out_ast, 0, sizeof(*out_ast));
// // 	out_ast->type = AST_CMD;
// // 	out_ast->as.cmd.argv = make_argv(2, "grep", "b", NULL, NULL, NULL);
// // 	out_ast->as.cmd.redirs = NULL;
// // 	push_redir(&out_ast->as.cmd.redirs, R_IN, "infile");   // デフォルトで fd 0
// // 	push_redir(&out_ast->as.cmd.redirs, R_OUT, "outfile"); // デフォルトで fd 1
// // 	out_ast->as.cmd.is_builtin = 0;
// // }

// // here_doc test
// static void	build_case1_grep(t_ast *out_ast)
// {
// 	t_redir	*r;

// 	memset(out_ast, 0, sizeof(*out_ast));
// 	out_ast->type = AST_CMD;
// 	out_ast->as.cmd.argv = make_argv(2, "grep", "b", NULL, NULL, NULL);
// 	out_ast->as.cmd.redirs = NULL;
// 	// ここを R_IN → R_HDOC に変更。limiter は例として "EOF"
// 	r = push_redir(&out_ast->as.cmd.redirs, R_HDOC, "EOF");
// 	// デフォルトで fd 0
// 	if (r)
// 	{
// 		r->quoted = 0;   // 0: 本文で $展開する / 1: 展開しない
// 		r->hdoc_fd = -1; // まだ未準備。prepare_heredocs で埋まる
// 							// r->fd_target は 0 のままでOK（stdin）
// 	}
// 	push_redir(&out_ast->as.cmd.redirs, R_OUT, "outfile"); // fd 1
// 	out_ast->as.cmd.is_builtin = 0;
// }

// // 2) /bin/ls -l
// static void	build_case2_ls(t_ast *out_ast)
// {
// 	memset(out_ast, 0, sizeof(*out_ast));
// 	out_ast->type = AST_CMD;
// 	out_ast->as.cmd.argv = make_argv(2, "/bin/ls", "-l", NULL, NULL, NULL);
// 	out_ast->as.cmd.redirs = NULL;
// 	out_ast->as.cmd.is_builtin = 0;
// }

// // 3) echo -n hello > out.txt（外部 echo 検証）
// static void	build_case3_echo(t_ast *out_ast, int as_builtin)
// {
// 	memset(out_ast, 0, sizeof(*out_ast));
// 	out_ast->type = AST_CMD;
// 	if (as_builtin)
// 	{
// 		out_ast->as.cmd.argv = make_argv(3, "echo", "-n", "hello", NULL, NULL);
// 		out_ast->as.cmd.is_builtin = 1;
// 	}
// 	else
// 	{
// 		out_ast->as.cmd.argv = make_argv(3, "/bin/echo", "-n", "hello", NULL,
// 				NULL);
// 		out_ast->as.cmd.is_builtin = 0;
// 	}
// 	out_ast->as.cmd.redirs = NULL;
// 	push_redir(&out_ast->as.cmd.redirs, R_OUT, "out.txt"); // 1 に向く
// }

// // 4) 左辺FD指定の検証：grep b NO_SUCH_FILE < infile > out.txt 2>> err.txt
// //    → stdout は out.txt、stderr は err.txt（追記）
// // static void	build_case4_fd_targets(t_ast *out_ast)
// // {
// // 	memset(out_ast, 0, sizeof(*out_ast));
// // 	out_ast->type = AST_CMD;
// // 	// 存在しないファイルを引数に入れて stderr を必ず発生させる
// // 	out_ast->as.cmd.argv = make_argv(3, "grep", "b", "NO_SUCH_FILE", NULL,
// // 			NULL);
// // 	out_ast->as.cmd.is_builtin = 0;
// // 	out_ast->as.cmd.redirs = NULL;
// // 	push_redir(&out_ast->as.cmd.redirs, R_IN, "infile");
// // 	// < infile      → fd 0
// // 	push_redir(&out_ast->as.cmd.redirs, R_OUT, "out.txt");
// // 	// > out.txt     → fd 1
// // 	push_redir_ex(&out_ast->as.cmd.redirs, R_APP, "err.txt", 2);
// // 	// 2>> err.txt   → fd 2
// // }

// // test_127
// static void	build_case4_fd_targets(t_ast *out_ast)
// {
// 	memset(out_ast, 0, sizeof(*out_ast));
// 	out_ast->type = AST_CMD;
// 	// 存在しないファイルを引数に入れて stderr を必ず発生させる
// 	out_ast->as.cmd.argv = make_argv(2, "nosuchcommand", NULL, NULL, NULL,
// 			NULL);
// 	out_ast->as.cmd.is_builtin = 0;
// 	out_ast->as.cmd.redirs = NULL;
// 	push_redir(&out_ast->as.cmd.redirs, R_IN, "infile");
// 	// < infile      → fd 0
// 	push_redir(&out_ast->as.cmd.redirs, R_OUT, "out.txt");
// 	// > out.txt     → fd 1
// 	push_redir_ex(&out_ast->as.cmd.redirs, R_APP, "err.txt", 2);
// 	// 2>> err.txt   → fd 2
// }

// // 5) 複数リダイレクト（後勝ち）の検証：echo -n hello > first.txt > second.txt 2> err.txt
// //    → 出力は最終的に second.txt のみ（first.txt は空 or 途中まで）
// static void	build_case5_multi_redirs(t_ast *out_ast)
// {
// 	memset(out_ast, 0, sizeof(*out_ast));
// 	out_ast->type = AST_CMD;
// 	out_ast->as.cmd.argv = make_argv(3, "/bin/echo", "-n", "hello", NULL, NULL);
// 	out_ast->as.cmd.is_builtin = 0;
// 	out_ast->as.cmd.redirs = NULL;
// 	push_redir(&out_ast->as.cmd.redirs, R_OUT, "first.txt");
// 	// > first.txt   → fd 1
// 	push_redir(&out_ast->as.cmd.redirs, R_OUT, "second.txt");
// 	// > second.txt  → fd 1（後勝ち）
// 	push_redir_ex(&out_ast->as.cmd.redirs, R_OUT, "err.txt", 2);
// 	// 2> err.txt    → fd 2
// }

// static void	build_case6_hdoc_basic(t_ast *out_ast)
// {
// 	memset(out_ast, 0, sizeof(*out_ast));
// 	out_ast->type = AST_CMD;
// 	out_ast->as.cmd.argv = make_argv(2, "/bin/cat", NULL, NULL, NULL, NULL);
// 	out_ast->as.cmd.is_builtin = 0;
// 	out_ast->as.cmd.redirs = NULL;
// 	push_redir_hdoc(&out_ast->as.cmd.redirs, "EOF", /*quoted=*/0);
// 	push_redir(&out_ast->as.cmd.redirs, R_OUT, "out.txt");
// 	// 入力例:
// 	//   hello
// 	//   world
// 	//   EOF
// 	// 期待: out.txt に "hello\nworld\n"
// }

// // 7) heredoc の後勝ち: wc -l <<AAA <<BBB
// static void	build_case7_hdoc_last_wins(t_ast *out_ast)
// {
// 	memset(out_ast, 0, sizeof(*out_ast));
// 	out_ast->type = AST_CMD;
// 	out_ast->as.cmd.argv = make_argv(3, "/bin/wc", "-l", NULL, NULL, NULL);
// 	out_ast->as.cmd.is_builtin = 0;
// 	out_ast->as.cmd.redirs = NULL;
// 	push_redir_hdoc(&out_ast->as.cmd.redirs, "AAA", 0);
// 	push_redir_hdoc(&out_ast->as.cmd.redirs, "BBB", 0);
// 	// BBB の内容だけが有効
// 	// 入力例:
// 	//  (AAA ブロック; 実際は無視される) AAA
// 	//  a
// 	//  b
// 	//  c
// 	//  BBB
// 	// 期待: "3\n"
// }

// // 8) quoted/unquoted の差（展開方針テスト用）: cat <<EOF / <<'EOF'
// static void	build_case8_hdoc_quoted_vs_unquoted(t_ast *out_ast, int quoted)
// {
// 	memset(out_ast, 0, sizeof(*out_ast));
// 	out_ast->type = AST_CMD;
// 	out_ast->as.cmd.argv = make_argv(2, "/bin/cat", NULL, NULL, NULL, NULL);
// 	out_ast->as.cmd.is_builtin = 0;
// 	out_ast->as.cmd.redirs = NULL;
// 	push_redir_hdoc(&out_ast->as.cmd.redirs, "EOF", quoted);
// 	// 入力例:
// 	//  hello $USER
// 	//  EOF
// 	// 期待:
// 	//  quoted==0 → 展開あり想定: "hello <user>\n"
// 	//  quoted==1 → 展開なし想定: "hello $USER\n"
// }

// // 9) 明示 fd_target: 0<<LIM と 2> err.txt 併用
// static void	build_case9_hdoc_target0_and_stderr(t_ast *out_ast)
// {
// 	memset(out_ast, 0, sizeof(*out_ast));
// 	out_ast->type = AST_CMD;
// 	out_ast->as.cmd.argv = make_argv(2, "/bin/cat", NULL, NULL, NULL, NULL);
// 	out_ast->as.cmd.is_builtin = 0;
// 	out_ast->as.cmd.redirs = NULL;
// 	push_redir_hdoc_ex(&out_ast->as.cmd.redirs, "LIM", 0, /*fd_target=*/0);
// 	push_redir_ex(&out_ast->as.cmd.redirs, R_OUT, "err.txt", /*fd_target=*/2);
// 	// 入力例:
// 	//  X
// 	//  Y
// 	//  LIM
// 	// 期待: STDOUT に "X\nY\n"、err.txt（通常は空）
// }

// // 11) パイプ: cat | grep b > outfile
// static void	build_case_pipe_grep_cat(t_ast *out_ast)
// {
// 	t_ast	*left;
// 	t_ast	*right;

// 	left = calloc(1, sizeof(t_ast));
// 	right = calloc(1, sizeof(t_ast));
// 	// 左: cat < infile
// 	left->type = AST_CMD;
// 	left->as.cmd.argv = make_argv(2, "/bin/cat", NULL, NULL, NULL, NULL);
// 	left->as.cmd.redirs = NULL;
// 	push_redir(&left->as.cmd.redirs, R_IN, "infile");
// 	left->as.cmd.is_builtin = 0;
// 	// 右: grep b > outfile
// 	right->type = AST_CMD;
// 	right->as.cmd.argv = make_argv(2, "grep", "b", NULL, NULL, NULL);
// 	right->as.cmd.redirs = NULL;
// 	push_redir(&right->as.cmd.redirs, R_OUT, "outfile");
// 	right->as.cmd.is_builtin = 0;
// 	// パイプノード
// 	memset(out_ast, 0, sizeof(*out_ast));
// 	out_ast->type = AST_PIPE;
// 	out_ast->as.pipe.left = left;
// 	out_ast->as.pipe.right = right;
// }

// static void	build_case_pipe_cat_grep_wc(t_ast *out_ast)
// {
// 	t_ast	*pipe1;

// 	t_ast *left, *mid, *right;
// 	// left: cat < infile
// 	left = calloc(1, sizeof(t_ast));
// 	left->type = AST_CMD;
// 	left->as.cmd.argv = make_argv(2, "/bin/cat", NULL, NULL, NULL, NULL);
// 	left->as.cmd.redirs = NULL;
// 	push_redir(&left->as.cmd.redirs, R_IN, "infile");
// 	left->as.cmd.is_builtin = 0;
// 	// mid: grep b
// 	mid = calloc(1, sizeof(t_ast));
// 	mid->type = AST_CMD;
// 	mid->as.cmd.argv = make_argv(2, "grep", "b", NULL, NULL, NULL);
// 	mid->as.cmd.redirs = NULL;
// 	mid->as.cmd.is_builtin = 0;
// 	// right: wc -l > outfile
// 	right = calloc(1, sizeof(t_ast));
// 	right->type = AST_CMD;
// 	right->as.cmd.argv = make_argv(3, "wc", "-l", NULL, NULL, NULL);
// 	right->as.cmd.redirs = NULL;
// 	push_redir(&right->as.cmd.redirs, R_OUT, "outfile");
// 	right->as.cmd.is_builtin = 0;
// 	// パイプノード（左: cat < infile | grep b, 右: wc -l > outfile）
// 	pipe1 = calloc(1, sizeof(t_ast));
// 	pipe1->type = AST_PIPE;
// 	pipe1->as.pipe.left = left;
// 	pipe1->as.pipe.right = mid;
// 	memset(out_ast, 0, sizeof(*out_ast));
// 	out_ast->type = AST_PIPE;
// 	out_ast->as.pipe.left = pipe1;
// 	out_ast->as.pipe.right = right;
// }

// ---- main -------------------------------------------------------------

int	main(int argc, char **argv, char **envp)
{
	t_shell		sh;
	t_ast		*root;
	t_tokvec	tv;
	int			which;
	int			status;

	ft_memset(&sh, 0, sizeof(sh));
	which = 1;
	status = 0;
	root = NULL;
	tv.vector = NULL;
	tv.len = 0;
	sh.envp = envp;
	sh.last_status = 0;
	sh.interactive = 0;
	if (argc >= 2)
		which = atoi(argv[1]);
	switch (which)
	{
	case 1:
	{
		static t_token case1[] = {
			{.args = "grep", .token_kind = TK_WORD, .fd_left = -1},
			{.args = "b", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_HEREDOC, .fd_left = -1, .hdoc_quoted = 0},
			{.args = "EOF", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_OUT, .fd_left = -1},
			{.args = "outfile", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_EOF, .fd_left = -1},
		};
		tv.vector = case1;
		tv.len = sizeof(case1) / sizeof(case1[0]);
		break ;
	}
	case 2:
	{
		static t_token case2[] = {
			{.args = "/bin/ls", .token_kind = TK_WORD, .fd_left = -1},
			{.args = "-l", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_EOF, .fd_left = -1},
		};
		tv.vector = case2;
		tv.len = sizeof(case2) / sizeof(case2[0]);
		break ;
	}
	case 3:
	{
		static t_token case3[] = {
			{.args = "/bin/echo", .token_kind = TK_WORD, .fd_left = -1},
			{.args = "-n", .token_kind = TK_WORD, .fd_left = -1},
			{.args = "hello", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_OUT, .fd_left = -1},
			{.args = "out.txt", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_EOF, .fd_left = -1},
		};
		tv.vector = case3;
		tv.len = sizeof(case3) / sizeof(case3[0]);
		break ;
	}
	case 4:
	{
		static t_token case4[] = {
			{.args = "nosuchcommand", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_IN, .fd_left = -1},
			{.args = "infile", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_OUT, .fd_left = -1},
			{.args = "out.txt", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_APP, .fd_left = 2},
			{.args = "err.txt", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_EOF, .fd_left = -1},
		};
		tv.vector = case4;
		tv.len = sizeof(case4) / sizeof(case4[0]);
		break ;
	}
	case 5:
	{
		static t_token case5[] = {
			{.args = "/bin/echo", .token_kind = TK_WORD, .fd_left = -1},
			{.args = "-n", .token_kind = TK_WORD, .fd_left = -1},
			{.args = "hello", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_OUT, .fd_left = -1},
			{.args = "first.txt", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_OUT, .fd_left = -1},
			{.args = "second.txt", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_OUT, .fd_left = 2},
			{.args = "err.txt", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_EOF, .fd_left = -1},
		};
		tv.vector = case5;
		tv.len = sizeof(case5) / sizeof(case5[0]);
		break ;
	}
	case 6:
	{
		static t_token case6[] = {
			{.args = "/bin/cat", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_HEREDOC, .fd_left = -1, .hdoc_quoted = 0},
			{.args = "EOF", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_OUT, .fd_left = -1},
			{.args = "out.txt", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_EOF, .fd_left = -1},
		};
		tv.vector = case6;
		tv.len = sizeof(case6) / sizeof(case6[0]);
		break ;
	}
	case 7:
	{
		static t_token case7[] = {
			{.args = "/bin/wc", .token_kind = TK_WORD, .fd_left = -1},
			{.args = "-l", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_HEREDOC, .fd_left = -1, .hdoc_quoted = 0},
			{.args = "AAA", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_HEREDOC, .fd_left = -1, .hdoc_quoted = 0},
			{.args = "BBB", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_EOF, .fd_left = -1},
		};
		tv.vector = case7;
		tv.len = sizeof(case7) / sizeof(case7[0]);
		break ;
	}
	case 8:
	{
		static t_token case8[] = {
			{.args = "/bin/cat", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_HEREDOC, .fd_left = -1, .hdoc_quoted = 0},
			{.args = "EOF", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_EOF, .fd_left = -1},
		};
		tv.vector = case8;
		tv.len = sizeof(case8) / sizeof(case8[0]);
		break ;
	}
	case 9:
	{
		static t_token case9[] = {
			{.args = "/bin/cat", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_HEREDOC, .fd_left = -1, .hdoc_quoted = 1},
			{.args = "EOF", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_EOF, .fd_left = -1},
		};
		tv.vector = case9;
		tv.len = sizeof(case9) / sizeof(case9[0]);
		break ;
	}
	case 10:
	{
		static t_token case10[] = {
			{.token_kind = TK_HEREDOC, .fd_left = 0, .hdoc_quoted = 0},
			{.args = "LIM", .token_kind = TK_WORD, .fd_left = -1},
			{.args = "/bin/cat", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_OUT, .fd_left = 2},
			{.args = "err.txt", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_EOF, .fd_left = -1},
		};
		tv.vector = case10;
		tv.len = sizeof(case10) / sizeof(case10[0]);
		break ;
	}
	case 11:
	{
		static t_token case11[] = {
			{.args = "/bin/cat", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_IN, .fd_left = -1},
			{.args = "infile", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_PIPE, .fd_left = -1},
			{.token_kind = TK_PIPE, .fd_left = -1},
			{.args = "grep", .token_kind = TK_WORD, .fd_left = -1},
			{.args = "b", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_OUT, .fd_left = -1},
			{.args = "outfile", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_EOF, .fd_left = -1},
		};
		tv.vector = case11;
		tv.len = sizeof(case11) / sizeof(case11[0]);
		break ;
	}
	case 12:
	{
		static t_token case12[] = {
			{.args = "/bin/cat", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_IN, .fd_left = -1},
			{.args = "infile", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_PIPE, .fd_left = -1},
			{.args = "grep", .token_kind = TK_WORD, .fd_left = -1},
			{.args = "b", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_PIPE, .fd_left = -1},
			{.args = "wc", .token_kind = TK_WORD, .fd_left = -1},
			{.args = "-l", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_REDIR_OUT, .fd_left = -1},
			{.args = "outfile", .token_kind = TK_WORD, .fd_left = -1},
			{.token_kind = TK_EOF, .fd_left = -1},
		};
		tv.vector = case12;
		tv.len = sizeof(case12) / sizeof(case12[0]);
		break ;
	}
	default:
		fprintf(stderr, "usage: %s [1..12]\n", argv[0]);
		fprintf(stderr, "  1: grep b <<EOF > outfile\n");
		fprintf(stderr, "  2: /bin/ls -l\n");
		fprintf(stderr, "  3: /bin/echo -n hello > out.txt\n");
		fprintf(stderr, "  4: nosuchcommand < infile > out.txt 2>> err.txt\n");
		fprintf(stderr,
			"  5:/bin/echo-n hello > first.txt > second.txt 2> err.txt\n");
		fprintf(stderr, "  6: /bin/cat <<EOF > out.txt\n");
		fprintf(stderr, "  7: /bin/wc -l <<AAA <<BBB\n");
		fprintf(stderr, "  8: /bin/cat <<EOF (unquoted)\n");
		fprintf(stderr, "  9: /bin/cat <<'EOF' (quoted)\n");
		fprintf(stderr, " 10: 0<<LIM /bin/cat 2> err.txt\n");
		fprintf(stderr, " 11: /bin/cat < infile | grep b > outfile\n");
		fprintf(stderr, " 12: /bin/cat < infile | grep b | wc -l > outfile\n");
		return (2);
	}
	root = make_ast(&tv, &sh);
	status = exec_entry(root, &sh);
	printf("[status] %d\n", status);
	free_ast(root);
	return (status != 0);
	// if (!tv.vector || tv.len == 0)
	// 	return (1);
	// if (!precheck_syntax(&tv))
	// 	return (1);
	// vector = tv.vector;
	// parse_limit = tv.len;
	// if (parse_limit > 0 && vector[parse_limit - 1].token_kind == TK_EOF)
	// 	parse_limit--;
	// idx = 0;
	// if (idx >= parse_limit)
	// 	return (1);
	// {
	// 	boundary = idx;
	// 	while (boundary < parse_limit && vector[boundary].token_kind != TK_PIPE)
	// 		boundary++;
	// 	root = parse_command(vector, boundary, &idx);
	// 	if (!root)
	// 		return (1);
	// 	while (idx < parse_limit)
	// 	{
	// 		if (vector[idx].token_kind != TK_PIPE)
	// 			return (free_ast(root), 1);
	// 		idx++;
	// 		next_boundary = idx;
	// 		while (next_boundary < parse_limit
	// 			&& vector[next_boundary].token_kind != TK_PIPE)
	// 			next_boundary++;
	// 		right = parse_command(vector, next_boundary, &idx);
	// 		if (!right)
	// 			return (free_ast(root), 1);
	// 		pipe_node = malloc(sizeof(*pipe_node));
	// 		if (!pipe_node)
	// 		{
	// 			free_ast(right);
	// 			free_ast(root);
	// 			perror("malloc");
	// 			return (1);
	// 		}
	// 		pipe_node->type = AST_PIPE;
	// 		pipe_node->as.pipe.left = root;
	// 		pipe_node->as.pipe.right = right;
	// 		root = pipe_node;
	// 	}
	// 	if (idx != parse_limit)
	// 	{
	// 		free_ast(root);
	// 		return (1);
	// 	}
	// }
}
