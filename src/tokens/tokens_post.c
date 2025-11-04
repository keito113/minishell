/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_post.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 15:03:39 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/04 09:17:09 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

static char	*join_parts_text(const t_wordinfo *w)
{
	size_t	len;
	char	*buf;
	size_t	i;
	size_t	k;

	len = parts_total_len(w);
	buf = malloc(len + 1);
	if (!buf)
		return (NULL);
	i = 0;
	k = 0;
	while (i < w->parts_count)
	{
		ft_memcpy(buf + k, w->parts[i].text, ft_strlen(w->parts[i].text));
		k += ft_strlen(w->parts[i].text);
		i++;
	}
	buf[k] = '\0';
	return (buf);
}

int	tok_is_redir(t_tok_kind k)
{
	if (k == TK_REDIR_IN || k == TK_REDIR_OUT)
		return (1);
	if (k == TK_REDIR_APP || k == TK_HEREDOC)
		return (1);
	return (0);
}

void	finalize_hdoc_flags(t_tokvec *tv)
{
	size_t		i;
	char		*lim;
	t_tok_kind	k;
	t_tok_kind	nk;

	i = 0;
	while (i + 1 < tv->len)
	{
		k = tv->vector[i].token_kind;
		nk = tv->vector[i + 1].token_kind;
		if (k == TK_HEREDOC && nk == TK_WORD)
		{
			tv->vector[i].hdoc_quoted = tv->vector[i + 1].word_info.had_quotes;
			lim = join_parts_text(&tv->vector[i + 1].word_info);
			if (lim)
			{
				if (tv->vector[i + 1].args)
					free(tv->vector[i + 1].args);
				tv->vector[i + 1].args = lim;
			}
		}
		i++;
	}
}

int	syntax_check(const t_tokvec *tv)
{
	size_t		i;
	t_tok_kind	k;
	t_tok_kind	nk;

	if (tv->len > 0 && tv->vector[0].token_kind == TK_PIPE)
		return (TOK_ERR_SYNTAX);
	i = 0;
	while (i + 1 < tv->len)
	{
		k = tv->vector[i].token_kind;
		nk = tv->vector[i + 1].token_kind;
		if (k == TK_PIPE && (nk == TK_PIPE || nk == TK_EOF))
			return (TOK_ERR_SYNTAX);
		if (tok_is_redir(k) && (nk == TK_EOF || nk == TK_PIPE
				|| tok_is_redir(nk)))
			return (TOK_ERR_SYNTAX);
		i++;
	}
	return (TOK_OK);
}

void	finalize_word_args(t_tokvec *tv)
{
	size_t	i;
	char	*str;

	i = 0;
	while (i < tv->len)
	{
		if (tv->vector[i].token_kind == TK_WORD)
		{
			str = join_parts_text(&tv->vector[i].word_info);
			if (str)
			{
				if (tv->vector[i].args)
					free(tv->vector[i].args);
				tv->vector[i].args = str;
			}
		}
		i++;
	}
}
