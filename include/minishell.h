#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

// ノード種別（AST）
typedef enum e_ast_type
{
	AST_PIPE,
	AST_CMD
}							t_ast_type;

// リダイレクト種別
typedef enum e_rtype
{
	R_IN,  // <
	R_OUT, // >
	R_APP, // >>
	R_HDOC // <<
}							t_rtype;

// 1件のリダイレクト
typedef struct s_redir
{
	t_rtype					kind;

	char *word; // filename or heredoc limiter（展開後/前の方針は後述）
	int quoted; // heredoc limiter がクォートされていたか（展開抑制用）
	int						hdoc_fd;
	struct s_redir			*next;
}							t_redir;

// 1コマンド（葉）
typedef struct s_cmd
{
	char **argv;     // 例: {"grep","foo",NULL}（展開・クォート除去後）
	t_redir *redirs; // このコマンドのリダイレクト連結リスト
	int is_builtin;  // 実行時の分岐に使うフラグ（0/1） or 別enum
}							t_cmd;

// ASTノード本体
typedef struct s_ast
{
	t_ast_type				type;
	union
	{
		struct
		{ // AST_PIPE 用
			struct s_ast	*left;
			struct s_ast	*right;
		} pipe;
		t_cmd *cmd; // AST_CMD 用
	} as;
}							t_ast;

// 実行時に必要な外部情報
typedef struct s_shell
{
	char **envp;     // execve用の環境
	int last_status; // $? の更新先
	int interactive; // 対話モードか
						// 必要なら: sigフラグや一時fdの退避など
}							t_shell;

int							exec_entry(t_ast *root, t_shell *sh);
int							run_single_command(t_cmd *cmd, t_shell *sh);
int							apply_redirs(const t_cmd *cmd);
void						exec_external(char *const argv[],
								char *const envp[]);
char						*find_cmd_path(const char *cmd, char *const *envp);
int							read_heredoc_into_fd(int write_fd, t_redir *redir,
								t_shell *sh);
int							prepare_cmd_heredocs(t_cmd *cmd, t_shell *sh,
								t_ast *node);
void						close_hdocs_in_cmd(t_cmd *cmd);
void						close_all_prepared_hdocs(t_ast *node);
int							status_to_exitcode(int wstatus);
void						close_all_prepared_hdocs(t_ast *node);

#endif
