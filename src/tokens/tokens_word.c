/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_word.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 09:52:21 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/15 11:26:40 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

static int	parts_grow_if_full(t_wbuild *wb)
{
	t_parts	*nv;
	size_t	ncap;

	if (wb->len != wb->cap)
		return (TOK_OK);
	if (wb->cap == 0)
		ncap = 4;
	else
		ncap = wb->cap * 2;
	nv = malloc(sizeof(t_parts) * ncap);
	if (!nv)
		return (TOK_ERR_OOM);
	if (wb->cap > 0)
		ft_memcpy(nv, wb->v, sizeof(t_parts) * wb->cap);
	free(wb->v);
	wb->v = nv;
	wb->cap = ncap;
	return (TOK_OK);
}

int	wb_begin_part(t_wbuild *wb, t_quote_kind q)
{
	char	*txt;
	int		rc;

	if (wb->blen > 0)
	{
		rc = parts_grow_if_full(wb);
		if (rc != TOK_OK)
			return (rc);
		txt = strndup_x(wb->buf, wb->blen);
		if (!txt)
			return (TOK_ERR_OOM);
		wb->v[wb->len].text = txt;
		wb->v[wb->len].quote = NO_QUOTE;
		wb->len++;
		wb->blen = 0;
	}
	if (q != NO_QUOTE)
		wb->had_quotes = 1;
	return (TOK_OK);
}

int	wb_putc(t_wbuild *wb, char c, int in_squote, int in_dquote)
{
	char	*nbuf;
	size_t	ncap;

	if (wb->blen == wb->bcap)
	{
		if (wb->bcap == 0)
			ncap = 32;
		else
			ncap = wb->bcap * 2;
		nbuf = malloc(ncap);
		if (!nbuf)
			return (TOK_ERR_OOM);
		if (wb->bcap > 0)
			ft_memcpy(nbuf, wb->buf, wb->blen);
		free(wb->buf);
		wb->buf = nbuf;
		wb->bcap = ncap;
	}
	wb->buf[wb->blen] = c;
	wb->blen++;
	if (!in_squote && c == '$')
		wb->had_dollar = 1;
	(void)in_dquote;
	return (TOK_OK);
}

int	wb_end_part(t_wbuild *wb, t_quote_kind q)
{
	int		rc;
	char	*txt;

	rc = parts_grow_if_full(wb);
	if (rc != TOK_OK)
		return (rc);
	txt = strndup_x(wb->buf, wb->blen);
	if (!txt)
		return (TOK_ERR_OOM);
	wb->v[wb->len].text = txt;
	wb->v[wb->len].quote = q;
	wb->len++;
	wb->blen = 0;
	if (q != NO_QUOTE)
		wb->had_quotes = 1;
	return (TOK_OK);
}

int	wb_emit_word_token(t_wbuild *wb, t_tokvec *out)
{
	t_token	tk;

	if (wb->blen > 0)
		wb_end_part(wb, NO_QUOTE);
	tk.token_kind = TK_WORD;
	tk.args = NULL;
	tk.word_info.parts = wb->v;
	tk.word_info.parts_count = wb->len;
	tk.word_info.had_dollar = wb->had_dollar;
	tk.word_info.had_quotes = wb->had_quotes;
	tk.fd_left = -1;
	tk.hdoc_quoted = 0;
	if (tokvec_push_word(out, &tk) != TOK_OK)
	{
		while (wb->len)
			free(wb->v[--wb->len].text);
		free(wb->v);
		return (TOK_ERR_OOM);
	}
	wb->v = NULL;
	wb->len = 0;
	wb->cap = 0;
	wb->had_dollar = 0;
	wb->had_quotes = 0;
	return (TOK_OK);
}
