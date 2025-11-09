/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 11:11:48 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/03 11:11:49 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "echo.h"

static int	is_n_option(const char *s)
{
	int	i;

	if (!s || s[0] != '-' || s[1] == '\0')
		return (0);
	i = 1;
	while (s[i] == 'n')
		i++;
	if (s[i] != '\0')
		return (0);
	return (1);
}

int	builtin_echo(char **av, int out_fd)
{
	int	i;
	int	no_nl;
	int	first;

	i = 1;
	no_nl = 0;
	first = 1;
	while (av[i] && is_n_option(av[i]))
	{
		no_nl = 1;
		i++;
	}
	while (av[i])
	{
		if (!first && write(out_fd, " ", 1) < 0)
			return (1);
		first = 0;
		if (write(out_fd, av[i], ft_strlen(av[i])) < 0)
			return (1);
		i++;
	}
	if (!no_nl && write(out_fd, "\n", 1) < 0)
		return (1);
	return (0);
}
