/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 10:53:33 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/01 14:27:57 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	add_digit_pos(long long *x, int d)
{
	if (*x > LLONG_MAX / 10)
		return (0);
	if (*x == LLONG_MAX / 10 && d > (int)(LLONG_MAX % 10))
		return (0);
	*x = (*x * 10) + d;
	return (1);
}

static int	add_digit_neg(long long *x, int d)
{
	if (*x < (LLONG_MIN + d) / 10)
		return (0);
	*x = (*x * 10) - d;
	return (1);
}

static int	parse_sign_and_first(const char **ps, int *psign)
{
	const char	*s;

	s = *ps;
	*psign = 1;
	if (*s == '+' || *s == '-')
	{
		if (*s == '-')
			*psign = -1;
		s++;
	}
	if (!ft_isdigit(*s))
		return (0);
	*ps = s;
	return (1);
}

static int	consume_digit(const char *s, int sign, long long *px)
{
	long long	x;
	int			d;

	x = 0;
	while (*s)
	{
		if (!ft_isdigit(*s))
			return (0);
		d = *s - '0';
		if (sign == 1)
		{
			if (!add_digit_pos(&x, d))
				return (0);
		}
		else
		{
			if (!add_digit_neg(&x, d))
				return (0);
		}
		s++;
	}
	*px = x;
	return (1);
}

int	str_to_ll_checked(const char *s, long long *out)
{
	long long	x;
	int			sign;

	if (!s || !out)
		return (0);
	if (!parse_sign_and_first(&s, &sign))
		return (0);
	if (!consume_digit(s, sign, &x))
		return (0);
	*out = x;
	return (1);
}
