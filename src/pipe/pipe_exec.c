/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 21:56:20 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/09 22:05:30 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	cmd_not_found_exit_pipe(const char *cmd, t_shell *sh,
		t_cmd **pipeline_cmds)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd((char *)cmd, STDERR_FILENO);
	ft_putendl_fd(": command not found", STDERR_FILENO);
	free(pipeline_cmds);
	cleanup_child_and_exit(sh, 127);
}

void	permission_denied_exit_pipe(const char *target, t_shell *sh,
		t_cmd **pipeline_cmds)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd((char *)target, STDERR_FILENO);
	ft_putendl_fd(": Permission denied", STDERR_FILENO);
	free(pipeline_cmds);
	cleanup_child_and_exit(sh, 126);
}

static void	exit_pipe_error(const char *cmd, t_shell *sh, t_cmd **pipeline_cmds)
{
	if (errno == ENOENT)
		cmd_not_found_exit_pipe(cmd, sh, pipeline_cmds);
	if (errno == EACCES || errno == EPERM)
		permission_denied_exit_pipe(cmd, sh, pipeline_cmds);
	free(pipeline_cmds);
	cleanup_child_and_exit(sh, 126);
}

void	validate_and_exec_pipe(char *path, char *const argv[], t_shell *sh,
		t_cmd **pipeline_cmds)
{
	struct stat	st;

	if (stat(path, &st) == -1)
	{
		free(path);
		exit_pipe_error(argv[0], sh, pipeline_cmds);
	}
	if (S_ISDIR(st.st_mode))
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(argv[0], STDERR_FILENO);
		ft_putendl_fd(": Is a directory", STDERR_FILENO);
		free(path);
		free(pipeline_cmds);
		cleanup_child_and_exit(sh, 126);
	}
	if (access(path, X_OK) != 0)
	{
		free(path);
		exit_pipe_error(argv[0], sh, pipeline_cmds);
	}
	execve(path, argv, sh->envp);
	free(path);
	exit_pipe_error(argv[0], sh, pipeline_cmds);
}

void	exec_external_pipe(char *const argv[], t_shell *sh,
		t_cmd **pipeline_cmds)
{
	char	*path;

	if (!argv || !argv[0])
	{
		free(pipeline_cmds);
		cleanup_child_and_exit(sh, 0);
	}
	path = find_cmd_path(argv[0], sh);
	if (!path)
	{
		free(pipeline_cmds);
		cmd_not_found_exit(argv[0], sh);
	}
	validate_and_exec_pipe(path, argv, sh, pipeline_cmds);
}
