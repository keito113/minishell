#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include "tokens.h"
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/select.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

# ifndef MINISHELL_FD_UPPER
#  define MINISHELL_FD_UPPER 1048576
# endif

// // TOKENの種類を判別
// typedef enum e_tok_kind
// {
// 	TK_WORD,      // コマンド名・引数・リダイレクトのターゲット（まだ展開前）
// 	TK_PIPE,      // |
// 	TK_REDIR_IN,  // <
// 	TK_REDIR_OUT, // >
// 	TK_REDIR_APP, // >>
// 	TK_HEREDOC,   // <<
// 	TK_EOF        // 入力末尾
// }							t_tok_kind;

// //クォートの種類を判別
// typedef enum e_quote_kind
// {
// 	NO_QUOTE,
// 	SINGLE,
// 	DOUBLE
// }							t_quote_kind;

// typedef struct s_parts
// {
// 	char *text;         // 生（クォートは外さない or 外すなら別マスク）
// 	t_quote_kind quote; // NO_QUOTE / SINGLE / DOUBLE
// }							t_parts;

// //トークンの中身を細かく分割した構造体
// typedef struct s_wordinfo
// {
// 	t_parts *parts;     // 連結で1語を構成（例:  a"$USER"'x' ）
// 	size_t parts_count; // parts の数
// 	int had_dollar;     // '$' を含む（SINGLE 内は無視するため後工程が最適化できる）
// 	int had_quotes;     // どこかにクォートがあった（quote-removalの有無判定に便利）
// }							t_wordinfo;

// /* 1トークン */
// typedef struct s_token
// {
// 	char *args;            //文字列
// 	t_tok_kind token_kind; // WORD/PIPE/RE_IN/OUT/APP/HEREDOC/EOF
// 	t_wordinfo word_info;  // 文字列とそれに紐づく情報
// 	int fd_left;           // 2> 等の左FD（なければ -1）
// 	int hdoc_quoted;       // TK_HEREDOC 用: リミッタがクォートされていたか
// }							t_token;

// //トークンの配列をしまう構造体
// typedef struct s_tokvec
// {
// 	t_token *vector; // token構造体の配列
// 	size_t len;      //配列の要素数
// }							t_tokvec;

// // ノード種別（AST）
// typedef enum e_ast_type
// {
// 	AST_PIPE,
// 	AST_CMD
// }							t_ast_type;

// // リダイレクト種別
// typedef enum e_rtype
// {
// 	R_IN,  // <
// 	R_OUT, // >
// 	R_APP, // >>
// 	R_HDOC // <<
// }							t_rtype;

// 1件のリダイレクト
typedef struct s_redir
{
	t_rtype					kind;
	char *arg; // filename or heredoc limiter（展開後/前の方針は後述）
	t_wordinfo				*word_info;
	int here_doc_quoted; // heredoc limiter がクォートされていたか（展開抑制用）
	int						fd_target;
	int						hdoc_fd;
	struct s_redir			*next;
}							t_redir;

// 1コマンド（葉）
typedef struct s_cmd
{
	char **argv; // 例: {"grep","foo",NULL}（展開・クォート除去後）
	t_wordinfo				**word_infos;
	size_t					argc;
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

//環境変数のテーブル
typedef struct s_env
{
	char *key;    // malloc所有
	char *val;    // malloc所有（NULL許可するなら要注意）
	int exported; // 0/1（print時はexportedなものを表示）
	struct s_env			*next;
}							t_env;

// 実行時に必要な外部情報
typedef struct s_shell
{
	char **envp;     // execve用の環境
	int last_status; // $? の更新先
	int interactive; // 対話モードか
	t_env					*env;
	// 必要なら: sigフラグや一時fdの退避など
}							t_shell;

// parse用の構造体
typedef struct s_parse_ctx
{
	const t_token			*tokens;
	size_t					len;
	size_t					index;
	t_shell					*sh;
}							t_parse_ctx;

// pipe用構造体
typedef struct s_pipe_ctx
{
	int prev_read;     // 前段から渡す読み端（バトン）
	int pipefd[2];     // 今回作るパイプ
	int need_pipe_out; // 今回の子は stdout→pipe?（1: はい / 0: いいえ）
	pid_t last_pid;    // 最後に fork した子（wait対象）
}							t_pipe_ctx;

// parse
t_ast						*parse(t_tokvec *tokenvec, t_shell *sh);
t_ast						*parse_command(const t_token *vector, size_t len,
								size_t *idx);
int							is_redir_op(t_tok_kind k);
int							precheck_syntax(const t_tokvec *tv, t_shell *sh);
int							push_word_to_argv(t_cmd *cmd, const t_token *token);
int							push_redir(t_cmd *cmd, const t_token *op_tok,
								const t_token *word_tok);
void						free_cmd(t_cmd *cmd);
void						free_argv(char **argv);
t_ast						*parse_command_fail(t_cmd *cmd);
int							syntax_fail(const t_token *tok, t_shell *sh);
void						free_ast(t_ast *node);
t_ast						*make_ast(t_tokvec *tokenvec, t_shell *shell);
int							append_wordinfo(t_cmd *cmd, const t_wordinfo *info);
t_wordinfo					*clone_wordinfo(const t_wordinfo *src);
void						free_wordinfo(t_wordinfo *info);
void						free_cmd_wordinfos(t_cmd *cmd);
t_wordinfo					*dup_wordinfo(const t_wordinfo *src);
int							push_word_info(t_cmd *cmd, const t_token *token);
void						free_wordinfo_dup(t_wordinfo *info);
int							is_invalid_fd(int fd);
int							report_bad_fd(int fd);

// expand
int							expand(t_ast *node, t_shell *sh);
char						*expand_word(const t_wordinfo *info, t_shell *sh);
char						*expand_chunk(const char *chunk, t_shell *sh);
char						*append_literal(const char *text);
const char					*lookup_env(const char *name, t_shell *sh);
char						*join_and_free(char *left, const char *right);
char						*join_num_and_free(char *left, int n);
int							is_valid_var_head(char c);
size_t						var_length(const char *str);

// exec
int							exec_entry(t_ast *root, t_shell *sh);
int							run_single_command(t_cmd *cmd, t_shell *sh);
int							run_pipeline(const t_ast *root, t_shell *sh);
int							apply_redirs(const t_cmd *cmd);
void						exec_external(char *const argv[], t_shell *sh);
char						*find_cmd_path(const char *cmd, t_shell *sh);
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
int							pipe_cmds_cleanup_and_return(t_cmd **pipeline_cmds,
								t_shell *sh);
int							pipe_error_cleanup(t_pipe_ctx *ctx,
								t_cmd **pipeline_cmds, size_t n, t_shell *sh);
int							build_pipeline_cmds(const t_ast *root,
								t_cmd ***out_seq, size_t *out_n, t_shell *sh);

// error
int							return_laststatus(t_shell *sh, int error_code);
int							status_to_exitcode(int wstatus);
void						cmd_not_found_exit(const char *cmd);
void						permission_denied_exit(const char *target);

// builtin
int							is_builtin_name(const char *name);
int							builtin_export(char **argv, t_env **penv);
int							handle_export_arg(char *arg, t_env **penv);
void						print_export_sorted(const t_env *env);
int							builtin_env(char **argv, t_env **env);

// env
t_env						*env_find(t_env *head, const char *key);
int							env_set(t_env **head, const char *key,
								const char *value, int exported);
int							env_append_value(t_env **head, const char *key,
								const char *suffix);
char						*dup_or_empty(const char *value);
int							env_init_from_envp(t_env **head, char **envp);
void						free_env_list(t_env **head);
void						env_list_append_node(t_env **head, t_env *node);

// builtin
int							exec_builtin(t_cmd *cmd, t_env **penv);
int							builtin_export(char **argv, t_env **penv);
int							builtin_env(char **argv, t_env **env);
int							builtin_pwd(char **argv);
int							builtin_unset(char **argv, t_env **penv);
int							is_valid_identifier(const char *s);

#endif