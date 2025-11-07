/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_error.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 14:46:44 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/07 14:26:58 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	cmd_not_found_exit(const char *cmd, t_shell *sh)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd((char *)cmd, STDERR_FILENO);
	ft_putendl_fd(": command not found", STDERR_FILENO);
	cleanup_child_and_exit(sh, 127);
	exit(127);
}

void	permission_denied_exit(const char *target, t_shell *sh)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd((char *)target, STDERR_FILENO);
	ft_putendl_fd(": Permission denied", STDERR_FILENO);
	cleanup_child_and_exit(sh, 126);
	exit(126);
}
