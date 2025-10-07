/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 11:44:56 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/02 11:57:20 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENS_H
# define TOKENS_H
# include "libft.h"
# include <stddef.h>

typedef enum e_tok_type
{
	TOK_WORD,
	TOK_PIPE,
	TOK_REDIR_IN,
	TOK_REDIR_OUT,
	TOK_REDIR_APP,
	TOK_HEREDOC,
	TOK_EOF
}				t_tok_type;

enum
{
	QM_NONE = 0u,
	QM_IN_SQ = 1u << 0,
	QM_IN_DQ = 1u << 1
};

typedef struct s_token
{
	t_tok_type	type;
	char *raw;      /* クォート込みの原文（除去は後段） */
	unsigned qmask; /* ' or " の内側だったか（展開可否に使う） */
	size_t pos;     /* エラー表示用開始位置（任意） */
	size_t		len;
}				t_token;

typedef enum e_lexerr
{
	LEX_OK = 0,
	LEX_UNCLOSED_QUOTE,
	LEX_BAD_SEQ,
	LEX_REDIR_MISSING
}				t_lexerr;

/* API（実装は後で） */
int				lex_line(const char *s, t_list **out_tokens, t_lexerr *err);
void			tok_free_list(t_list **lst);

#endif
