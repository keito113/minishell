/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_utils_fd.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 14:05:12 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/20 15:26:46 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

static int	fd_within_intmax(const char *s, size_t p)
{
	size_t		i;
	const char	*mx;

	i = 0;
	mx = "2147483647";
	while (i < 10 && s[p + i] == mx[i])
		i++;
	if (i < 10 && (unsigned char)s[p + i] > (unsigned char)mx[i])
		return (0);
	return (1);
}

static int	fd_accumulate(const char *s, size_t p, size_t r, int *out_fd)
{
	int		acc;
	size_t	i;

	acc = 0;
	i = p;
	while (i <= r)
	{
		acc *= 10;
		acc += s[i] - '0';
		i++;
	}
	*out_fd = acc;
	return (1);
}

int	parse_fd_int_slice(const char *s, size_t l, size_t r, int *out_fd)
{
	size_t	p;
	size_t	len;

	p = l;
	while (p <= r && s[p] == '0')
		p++;
	if (p > r)
	{
		*out_fd = 0;
		return (1);
	}
	len = r - p + 1;
	if (len > 10)
		return (0);
	if (len == 10 && !fd_within_intmax(s, p))
		return (0);
	return (fd_accumulate(s, p, r, out_fd));
}

int	read_fd_left(const char *s, size_t op_pos, int *out_fd)
{
	ssize_t	r;
	ssize_t	l;
	int		res;
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
	res = parse_fd_int_slice(s, (size_t)l, (size_t)r, out_fd);
	return (res);
}
