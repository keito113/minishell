/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bad_fd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 16:35:08 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/22 16:39:58 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	report_bad_fd(int fd)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putnbr_fd(fd, STDERR_FILENO);
	ft_putendl_fd(": Bad file descriptor", STDERR_FILENO);
	return (-1);
}

int	is_invalid_fd(int fd)
{
	if (fd < 0)
		return (0);
	if (fd >= MINISHELL_FD_UPPER)
		return (1);
	return (0);
}
