/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 17:25:56 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/14 11:05:25 by keitabe          ###   ########.fr       */
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

//呼び出し側が | < > のいずれかを確認済みで呼ぶ。
//そうでない場合に '>' 以外を TK_REDIR_OUT にしてしまうので、lexer 側で「演算子先頭」を検知してから呼ぶ。
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

int	read_fd_left(const char *s, size_t op_pos, int *out_fd)
{
	ssize_t	r;
	ssize_t	l;
	char	*tmp;
	char	c;

	if (op_pos == 0 || !ft_isdigit((unsigned char)s[op_pos - 1]))
		return (0);
	r = (ssize_t)op_pos - 1;
	l = r;
	while (l >= 0 && ft_isdigit((unsigned char)s[l]))
		l--;
	l++;
	if (l > 0)
	{
		c = s[l - 1];
		if (!is_space_tab(c) && c != '|' && c != '<' && c != '>')
			return (0);
	}
	tmp = strndup_x(s + l, (size_t)(r - l + 1));
	if (!tmp)
		return (0);
	*out_fd = ft_atoi(tmp);
	free(tmp);
	return (1);
}
