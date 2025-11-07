/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:05:28 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/07 14:28:26 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	exit_exec_error(const char *cmd, t_shell *sh)
{
	if (errno == ENOENT)
		cmd_not_found_exit(cmd, sh);
	if (errno == EACCES || errno == EPERM)
		permission_denied_exit(cmd, sh);
	exit(126);
}

static void	validate_and_exec(char *path, char *const argv[], t_shell *sh)
{
	struct stat	st;

	if (stat(path, &st) == -1)
	{
		free(path);
		exit_exec_error(argv[0], sh);
	}
	if (S_ISDIR(st.st_mode))
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(argv[0], STDERR_FILENO);
		ft_putendl_fd(": Is a directory", STDERR_FILENO);
		free(path);
		exit(126);
	}
	if (access(path, X_OK) != 0)
	{
		free(path);
		exit_exec_error(argv[0], sh);
	}
	execve(path, argv, sh->envp);
	free(path);
	exit_exec_error(argv[0], sh);
}

void	exec_external(char *const argv[], t_shell *sh)
{
	char	*path;

	if (!argv || !argv[0])
		exit(0);
	path = find_cmd_path(argv[0], sh);
	if (!path)
		cmd_not_found_exit(argv[0], sh);
	validate_and_exec(path, argv, sh);
}
