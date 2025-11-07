/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:20:51 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/07 18:59:50 by takawagu         ###   ########.fr       */
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

typedef enum e_tok_kind
{
	TK_WORD,      /* コマンド名・引数・リダイレクトのターゲット（展開前） */
	TK_PIPE,      /* | */
	TK_REDIR_IN,  /* < */
	TK_REDIR_OUT, /* > */
	TK_REDIR_APP, /* >> */
	TK_HEREDOC,   /* << */
	TK_EOF        /* 入力末尾 */
}						t_tok_kind;

/* クォートの種類を判別 */
typedef enum e_quote_kind
{
	NO_QUOTE,
	SINGLE,
	DOUBLE
}						t_quote_kind;

/* 語を構成する断片（クォート境界ごと） */
typedef struct s_parts
{
	char *text;         /* クォートは除去済みの中身（quote-removal前でもOK） */
	t_quote_kind quote; /* NO_QUOTE / SINGLE / DOUBLE */
}						t_parts;

/* 1語の追加情報 */
typedef struct s_wordinfo
{
	t_parts *parts;     /* 連結で1語を構成（例: a"$USER"'x' ） */
	size_t parts_count; /* parts の数 */
	int had_dollar;     /* '$' を含む（SINGLE 内は無視） */
	int had_quotes;     /* どこかにクォートがあった */
}						t_wordinfo;

/* 1トークン */
typedef struct s_token
{
	char *args;            /* 表示/デバッグ用に結合した文字列 */
	t_tok_kind token_kind; /* WORD/PIPE/RE_IN/OUT/APP/HEREDOC/EOF */
	t_wordinfo word_info;  /* 文字列の内訳（展開に使用） */
	int fd_left;           /* 2> 等の左FD（なければ -1） */
	int hdoc_quoted;       /* TK_HEREDOC 用: リミッタがクォートされていたか */
}						t_token;

typedef struct s_tokvec
{
	t_token				*vector;
	size_t				len;
	size_t				cap;
}						t_tokvec;

/* ノード種別（AST）— 先読みで使うのでここに置く（必要なら後で ast.h へ分離） */
typedef enum e_ast_type
{
	AST_PIPE,
	AST_CMD
}						t_ast_type;

/* リダイレクト種別 */
typedef enum e_rtype
{
	R_IN,  /* < */
	R_OUT, /* > */
	R_APP, /* >> */
	R_HDOC /* << */
}						t_rtype;

/* エラーコード（0=OK） */
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
	t_parts *v;     // parts 動的配列（t_parts は tokens.h 既定）
	size_t len;     // 使用要素数
	size_t cap;     // 確保要素数（倍増戦略）
	char *buf;      // 現在書き込み中の断片バッファ
	size_t blen;    // buf の使用バイト数
	size_t bcap;    // buf の確保バイト数
	int had_dollar; // '$' を（SINGLE以外で）踏んだフラグ
	int had_quotes; // どこかでクォートを使ったフラグ
}						t_wbuild;

/* レキサ1行処理の最小コンテキスト（内部用） */
typedef struct s_lexctx
{
	const char *s; /* 入力行 */
	size_t i;      /* 現在位置（文字インデックス） */
	int st;        /* LXS_NORMAL / LXS_IN_SQ / LXS_IN_DQ */
	t_wbuild wb;   /* ワードビルダー（再入用・ローカル） */
	t_tokvec *out; /* 出力トークンベクタ */
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

#endif
