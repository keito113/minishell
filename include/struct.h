/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:20:51 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/10 07:54:35 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCT_H
# define STRUCT_H

# include "libft.h"
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
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

typedef struct s_ast	t_ast;
typedef struct s_cmd	t_cmd;

typedef enum e_tok_kind
{
	TK_WORD,
	TK_PIPE,
	TK_REDIR_IN,
	TK_REDIR_OUT,
	TK_REDIR_APP,
	TK_HEREDOC,
	TK_EOF
}						t_tok_kind;

typedef enum e_quote_kind
{
	NO_QUOTE,
	SINGLE,
	DOUBLE
}						t_quote_kind;

typedef struct s_parts
{
	char				*text;
	t_quote_kind		quote;
}						t_parts;

typedef struct s_wordinfo
{
	t_parts				*parts;
	size_t				parts_count;
	int					had_dollar;
	int					had_quotes;
}						t_wordinfo;

typedef struct s_token
{
	char				*args;
	t_tok_kind			token_kind;
	t_wordinfo			word_info;
	int					fd_left;
	int					hdoc_quoted;
}						t_token;

typedef struct s_tokvec
{
	t_token				*vector;
	size_t				len;
	size_t				cap;
}						t_tokvec;

typedef enum e_ast_type
{
	AST_PIPE,
	AST_CMD
}						t_ast_type;

typedef enum e_rtype
{
	R_IN,
	R_OUT,
	R_APP,
	R_HDOC
}						t_rtype;

typedef enum e_tokerr
{
	TOK_OK = 0,
	TOK_ERR_UNCLOSED_SQUOTE,
	TOK_ERR_UNCLOSED_DQUOTE,
	TOK_ERR_SYNTAX,
	TOK_ERR_OOM
}						t_tokerr;

typedef enum e_lex_state
{
	LXS_NORMAL,
	LXS_IN_SQ,
	LXS_IN_DQ
}						t_lex_state;

typedef struct s_wbuild
{
	t_parts				*v;
	size_t				len;
	size_t				cap;
	char				*buf;
	size_t				blen;
	size_t				bcap;
	int					had_dollar;
	int					had_quotes;
}						t_wbuild;

typedef struct s_lexctx
{
	const char			*s;
	size_t				i;
	int					st;
	t_wbuild			wb;
	t_tokvec			*out;
}						t_lexctx;

typedef struct s_env
{
	char				*key;
	char				*val;
	int					exported;
	struct s_env		*next;
}						t_env;

typedef struct s_shell
{
	char				**envp;
	int					last_status;
	int					interactive;
	t_env				*env;
	t_ast				*currrent_ast;
	int					exit_status;
	int					should_exit;
	t_cmd				**pipeline_cmds;
}						t_shell;

typedef struct s_redir
{
	t_rtype				kind;
	char				*arg;
	t_wordinfo			*word_info;
	int					here_doc_quoted;
	int					fd_target;
	int					hdoc_fd;
	struct s_redir		*next;
}						t_redir;

typedef struct s_cmd
{
	char				**argv;
	t_wordinfo			**word_infos;
	size_t				argc;
	t_redir				*redirs;
	int					is_builtin;
	int					is_child;
	t_shell				*sh;
}						t_cmd;

typedef struct s_ast_pipe
{
	struct s_ast		*left;
	struct s_ast		*right;
}						t_ast_pipe;

typedef union u_ast_payload
{
	t_ast_pipe			pipe;
	t_cmd				cmd;
}						t_ast_payload;

typedef struct s_ast
{
	t_ast_type			type;
	t_ast_payload		as;
}						t_ast;

typedef struct s_parse_ctx
{
	const t_token		*tokens;
	size_t				len;
	size_t				index;
	t_shell				*sh;
}						t_parse_ctx;

typedef struct s_pipe_ctx
{
	int					prev_read;
	int					pipefd[2];
	int					need_pipe_out;
	pid_t				last_pid;
}						t_pipe_ctx;

typedef struct s_fd_backup
{
	int					target_fd;
	int					saved_fd;
}						t_fd_backup;

typedef struct s_pathvec
{
	char				**segments;
	size_t				size;
	size_t				cap;
}						t_pathvec;

typedef struct s_expand_ctx
{
	t_cmd				*cmd;
	t_shell				*sh;
	const char			*ifs;
}						t_expand_ctx;

typedef struct s_insert_ctx
{
	t_cmd				*cmd;
	char				**new_argv;
	t_wordinfo			**new_infos;
	size_t				nf;
}						t_insert_ctx;

#endif
