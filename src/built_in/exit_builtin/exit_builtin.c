/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 10:48:57 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/10 07:58:40 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	exit_count_args(char **av)
{
	int	i;

	i = 0;
	while (av[i])
		i++;
	return (i);
}

static int	exit_request(t_shell *sh, int is_child, int code, int print_exit)
{
	if (!is_child)
	{
		if (sh)
		{
			if (print_exit && sh->interactive)
				ft_putendl_fd("exit", STDOUT_FILENO);
			sh->exit_status = code;
			sh->should_exit = 1;
		}
		return (0);
	}
	cleanup_child_and_exit(sh, code);
	return (0);
}

static void	print_num_required(const char *arg)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd((char *)arg, STDERR_FILENO);
	ft_putendl_fd(": numeric argument required", STDERR_FILENO);
}

static void	print_too_many(void)
{
	ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
}

int	builtin_exit(char **av, t_shell *sh, int is_child)
{
	int			ac;
	long long	n;
	int			ok;

	ac = exit_count_args(av);
	if (ac == 1)
		return (exit_request(sh, is_child, (unsigned char)sh->last_status, 1));
	ok = str_to_ll_checked(av[1], &n);
	if (!ok)
	{
		print_num_required(av[1]);
		return (exit_request(sh, is_child, 255, 1));
	}
	if (ac > 2)
	{
		print_too_many();
		sh->last_status = 1;
		return (1);
	}
	return (exit_request(sh, is_child, (unsigned char)n, 1));
}
