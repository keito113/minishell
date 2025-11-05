/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_lex_helpers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 11:45:51 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/05 14:14:37 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

int	lex_handle_space_or_end(t_wbuild *wb, t_tokvec *out)
{
	int	rc;

	if (wb->blen > 0 || wb->len > 0)
	{
		rc = wb_emit_word_token(wb, out);
		if (rc != TOK_OK)
			return (rc);
	}
	return (TOK_OK);
}

int	lex_handle_operator(const char *s, size_t *i, t_wbuild *wb, t_tokvec *out)
{
	t_tok_kind	k;
	size_t		oplen;
	int			fd;
	int			rc;

	read_op(s, *i, &k, &oplen);
	fd = -1;
	read_fd_left(s, *i, &fd);
	if (fd != -1 && wb->len == 0)
	{
		wb->blen = 0;
	}
	else
	{
		rc = lex_handle_space_or_end(wb, out);
		if (rc != TOK_OK)
			return (rc);
		if (fd != -1)
			fd = -1;
	}
	rc = tokvec_push_op(out, k, fd, 0);
	if (rc != TOK_OK)
		return (rc);
	*i += oplen;
	return (TOK_OK);
}

int	lex_enter_quote(t_quote_kind qkind, t_wbuild *wb, int *st)
{
	int	rc;

	rc = wb_begin_part(wb, qkind);
	if (rc != TOK_OK)
		return (rc);
	if (qkind == SINGLE)
		*st = LXS_IN_SQ;
	else
		*st = LXS_IN_DQ;
	return (TOK_OK);
}

int	lex_handle_char(char c, int st, t_wbuild *wb)
{
	int	in_sq;
	int	in_dq;

	in_sq = 0;
	in_dq = 0;
	if (st == LXS_IN_SQ)
		in_sq = 1;
	else if (st == LXS_IN_DQ)
		in_dq = 1;
	return (wb_putc(wb, c, in_sq, in_dq));
}

void	wb_cleanup(t_wbuild *wb)
{
	size_t	i;

	if (wb->v != NULL)
	{
		i = 0;
		while (i < wb->len)
		{
			free(wb->v[i].text);
			i++;
		}
		free(wb->v);
	}
	free(wb->buf);
	wb->v = NULL;
	wb->len = 0;
	wb->buf = NULL;
	wb->blen = 0;
}
