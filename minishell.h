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
	char *arg;  // filename or heredoc limiter（展開後/前の方針は後述）
	int quoted; // heredoc limiter がクォートされていたか（展開抑制用）
	int						fd_target;
	int						hdoc_fd;
	struct s_redir			*next;
}							t_redir;

// 1コマンド（葉）
typedef struct s_cmd
{
	char **argv;     // 例: {"grep","foo",NULL}（展開・クォート除去後）
	t_redir *redirs; // このコマンドのリダイレクト連結リスト
	int is_builtin;  // 実行時の分岐に使うフラグ（0/1） or 別enum
	int is_child;    //子プロセスの中かどうかの判定
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
		t_cmd cmd; // AST_CMD 用
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

// pipe用構造体
typedef struct s_pipe_ctx
{
	int prev_read;     // 前段から渡す読み端（バトン）
	int pipefd[2];     // 今回作るパイプ
	int need_pipe_out; // 今回の子は stdout→pipe?（1: はい / 0: いいえ）
	pid_t last_pid;    // 最後に fork した子（wait対象）
}							t_pipe_ctx;

int							exec_entry(t_ast *root, t_shell *sh);
int							run_single_command(t_cmd *cmd, t_shell *sh);
int							run_pipeline(const t_ast *root, t_shell *sh);
int							apply_redirs(const t_cmd *cmd);
void						exec_external(char *const argv[],
								char *const envp[]);
char						*find_cmd_path(const char *cmd, char *const *envp);
int							open_src_fd(t_redir *redir);
int							read_heredoc_into_fd(int write_fd, t_redir *redir,
								t_shell *sh);
int							prepare_cmd_heredocs(t_cmd *cmd, t_shell *sh,
								t_ast *node);
void						close_hdocs_in_cmd(t_cmd *cmd);
void						close_all_prepared_hdocs(t_ast *node);
void						close_hdocs_in_cmd(t_cmd *cmd);
void						close_all_prepared_hdocs(t_ast *node);

// pipe
int							reap_pipeline_and_set_last_status(pid_t last_pid,
								t_shell *sh);

void						pipe_ctx_init(t_pipe_ctx *ctx);
int							pipe_ctx_prepare_step(t_pipe_ctx *ctx, size_t i,
								size_t n);
void						pipe_ctx_parent_after_fork(t_pipe_ctx *ctx);

void						close_hdocs_in_cmd(t_cmd *cmd);
int							prepare_all_heredocs(t_cmd **pipeline_cmds,
								size_t n, t_shell *sh);
void						close_hdocs_in_pipeline(t_cmd **pipeline_cmds,
								size_t n);
void						run_pipeline_child(t_pipe_ctx *pipe_ctx,
								t_cmd **pipeline_cmds, size_t i, t_shell *sh);
int							pipeline_cmds_cleanup_and_return(t_cmd **pipeline_cmds,
								t_shell *sh);
int							pipeline_error_cleanup(t_pipe_ctx *ctx,
								t_cmd **pipeline_cmds, size_t n, t_shell *sh);
int							build_pipeline_cmds(const t_ast *root,
								t_cmd ***out_seq, size_t *out_n, t_shell *sh);

// error
int							return_laststatus(t_shell *sh, int error_code);
int							status_to_exitcode(int wstatus);

#endif