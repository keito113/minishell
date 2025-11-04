/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_vec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 14:05:05 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/04 08:12:18 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

void	tokvec_init(t_tokvec *v)
{
	v->vector = NULL;
	v->len = 0;
	v->cap = 0;
}

static int	grow_if_full(t_tokvec *v)
{
	t_token	*nv;
	size_t	ncap;

	if (v->len < v->cap)
		return (TOK_OK);
	ncap = 8;
	if (v->cap > 0)
		ncap = v->cap * 2;
	nv = malloc(sizeof(*nv) * ncap);
	if (!nv)
		return (TOK_ERR_OOM);
	if (v->len > 0 && v->vector)
		ft_memcpy(nv, v->vector, sizeof(*nv) * v->len);
	free(v->vector);
	v->vector = nv;
	v->cap = ncap;
	return (TOK_OK);
}

int	tokvec_push_word(t_tokvec *v, t_wordinfo w)
{
	int		rc;
	t_token	*slot;

	rc = grow_if_full(v);
	if (rc != TOK_OK)
		return (rc);
	slot = &v->vector[v->len];
	ft_bzero(slot, sizeof(slot));
	slot->token_kind = TK_WORD;
	slot->word_info = w;
	slot->fd_left = -1;
	v->len++;
	return (TOK_OK);
}

int	tokvec_push_op(t_tokvec *v, t_tok_kind kind, int fd_left, int hdoc_quoted)
{
	t_token	tk;
	int		rc;

	rc = grow_if_full(v);
	if (rc != TOK_OK)
		return (rc);
	tk.args = NULL;
	tk.token_kind = kind;
	tk.word_info.parts = NULL;
	tk.word_info.parts_count = 0;
	tk.word_info.had_dollar = 0;
	tk.word_info.had_quotes = 0;
	tk.fd_left = fd_left;
	tk.hdoc_quoted = hdoc_quoted;
	v->vector[v->len++] = tk;
	return (TOK_OK);
}

int	tokvec_push_eof(t_tokvec *v)
{
	return (tokvec_push_op(v, TK_EOF, -1, 0));
}
