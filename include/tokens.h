/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 11:44:56 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/20 15:24:00 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENS_H
# define TOKENS_H

# include "libft.h"
# include <stddef.h>
# include <stdlib.h>
# include <sys/types.h>

/* TOKENの種類を判別 */
typedef enum e_tok_kind
{
	TK_WORD,      /* コマンド名・引数・リダイレクトのターゲット（展開前） */
	TK_PIPE,      /* | */
	TK_REDIR_IN,  /* < */
	TK_REDIR_OUT, /* > */
	TK_REDIR_APP, /* >> */
	TK_HEREDOC,   /* << */
	TK_EOF        /* 入力末尾 */
}			t_tok_kind;

/* クォートの種類を判別 */
typedef enum e_quote_kind
{
	NO_QUOTE,
	SINGLE,
	DOUBLE
}			t_quote_kind;

/* 語を構成する断片（クォート境界ごと） */
typedef struct s_parts
{
	char *text;         /* クォートは除去済みの中身（quote-removal前でもOK） */
	t_quote_kind quote; /* NO_QUOTE / SINGLE / DOUBLE */
}			t_parts;

/* 1語の追加情報 */
typedef struct s_wordinfo
{
	t_parts *parts;     /* 連結で1語を構成（例: a"$USER"'x' ） */
	size_t parts_count; /* parts の数 */
	int had_dollar;     /* '$' を含む（SINGLE 内は無視） */
	int had_quotes;     /* どこかにクォートがあった */
}			t_wordinfo;

/* 1トークン */
typedef struct s_token
{
	char *args;            /* 表示/デバッグ用に結合した文字列 */
	t_tok_kind token_kind; /* WORD/PIPE/RE_IN/OUT/APP/HEREDOC/EOF */
	t_wordinfo word_info;  /* 文字列の内訳（展開に使用） */
	int fd_left;           /* 2> 等の左FD（なければ -1） */
	int hdoc_quoted;       /* TK_HEREDOC 用: リミッタがクォートされていたか */
}			t_token;

typedef struct s_tokvec
{
	t_token	*vector;
	size_t	len;
	size_t	cap;
}			t_tokvec;

/* ノード種別（AST）— 先読みで使うのでここに置く（必要なら後で ast.h へ分離） */
typedef enum e_ast_type
{
	AST_PIPE,
	AST_CMD
}			t_ast_type;

/* リダイレクト種別 */
typedef enum e_rtype
{
	R_IN,  /* < */
	R_OUT, /* > */
	R_APP, /* >> */
	R_HDOC /* << */
}			t_rtype;

/* エラーコード（0=OK） */
typedef enum e_tokerr
{
	TOK_OK = 0,
	TOK_ERR_UNCLOSED_SQUOTE,
	TOK_ERR_UNCLOSED_DQUOTE,
	TOK_ERR_SYNTAX,
	TOK_ERR_OOM
}			t_tokerr;

typedef enum e_lex_state
{
	LXS_NORMAL,
	LXS_IN_SQ,
	LXS_IN_DQ
}			t_lex_state;

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
}			t_wbuild;

/* レキサ1行処理の最小コンテキスト（内部用） */
typedef struct s_lexctx
{
	const char *s; /* 入力行 */
	size_t i;      /* 現在位置（文字インデックス） */
	int st;        /* LXS_NORMAL / LXS_IN_SQ / LXS_IN_DQ */
	t_wbuild wb;   /* ワードビルダー（再入用・ローカル） */
	t_tokvec *out; /* 出力トークンベクタ */
}			t_lexctx;

// tokens_utils.c
void		wb_init(t_wbuild *wb);
int			is_space_tab(char c);
void		read_op(const char *s, size_t i, t_tok_kind *k, size_t *len);
char		*strndup_x(const char *p, size_t n);

// tokens_utils_fd.c
int			parse_fd_int_slice(const char *s, size_t l, size_t r, int *out_fd);
int			read_fd_left(const char *s, size_t op_pos, int *out_fd);

// tokens_word.c
int			wb_begin_part(t_wbuild *wb, t_quote_kind q);
int			wb_putc(t_wbuild *wb, char c, int in_squote, int in_dquote);
int			wb_end_part(t_wbuild *wb, t_quote_kind q);
int			wb_emit_word_token(t_wbuild *wb, t_tokvec *out);

// tokens_vec_free.c
void		tokvec_free(t_tokvec *v);

// tokens_vec.c
void		tokvec_init(t_tokvec *v);
int			tokvec_push_word(t_tokvec *v, const t_token *src);
int			tokvec_push_op(t_tokvec *v, t_tok_kind kind, int fd_left,
				int hdoc_quoted);
int			tokvec_push_eof(t_tokvec *v);

// tokens_lex_helpers.c
int			lex_handle_space_or_end(t_wbuild *wb, t_tokvec *out);
int			lex_handle_operator(const char *s, size_t *i, t_wbuild *wb,
				t_tokvec *out);
int			lex_enter_quote(t_quote_kind qkind, t_wbuild *wb, int *st);
int			lex_handle_char(char c, int st, t_wbuild *wb);
void		wb_cleanup(t_wbuild *wb);

// tokens_lex.c
int			tok_lex_line(const char *s, t_tokvec *out, int *err);

// tokens_post.c
int			tok_is_redir(t_tok_kind k);
void		finalize_hdoc_flags(t_tokvec *tv);
int			syntax_check(const t_tokvec *tv);

#endif
