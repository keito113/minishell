/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 17:25:56 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/20 15:24:09 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

void	wb_init(t_wbuild *wb)
{
	wb->v = NULL;
	wb->len = 0;
	wb->cap = 0;
	wb->buf = NULL;
	wb->blen = 0;
	wb->bcap = 0;
	wb->had_dollar = 0;
	wb->had_quotes = 0;
	return ;
}

int	is_space_tab(char c)
{
	if (c == ' ' || c == '\t')
		return (1);
	return (0);
}

void	read_op(const char *s, size_t i, t_tok_kind *k, size_t *len)
{
	*len = 1;
	if (s[i] == '|')
		*k = TK_PIPE;
	else if (s[i] == '<')
	{
		*k = TK_REDIR_IN;
		if (s[i + 1] == '<')
		{
			*k = TK_HEREDOC;
			*len = 2;
		}
	}
	else
	{
		*k = TK_REDIR_OUT;
		if (s[i] == '>' && s[i + 1] == '>')
		{
			*k = TK_REDIR_APP;
			*len = 2;
		}
	}
}

char	*strndup_x(const char *p, size_t n)
{
	char	*copy;
	size_t	i;

	copy = malloc(n + 1);
	if (!copy)
		return (NULL);
	i = 0;
	while (i < n && p[i])
	{
		copy[i] = p[i];
		i++;
	}
	copy[i] = '\0';
	return (copy);
}
