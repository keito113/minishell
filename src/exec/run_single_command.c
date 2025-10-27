/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_single_command.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:06:06 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/27 15:23:04 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	exec_child_single(t_cmd *cmd, t_shell *sh)
{
	if (apply_redirs(cmd) < 0)
		exit(1);
	exec_external(cmd->argv, sh);
}

static int	wait_child_and_set_status(pid_t pid, t_shell *sh)
{
	int	status;

	while (waitpid(pid, &status, 0) < 0)
	{
		if (errno == EINTR)
			continue ;
		perror("waitpid");
		return (sh->last_status = 1);
	}
	return (sh->last_status = status_to_exitcode(status));
}

int	run_single_command(t_cmd *cmd, t_shell *sh)
{
	pid_t	pid;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (sh->last_status = 1);
	if (prepare_cmd_heredocs(cmd, sh, NULL) != 0)
		return (close_hdocs_in_cmd(cmd), sh->last_status);
	if (cmd->is_builtin)
	{
		close_hdocs_in_cmd(cmd);
		return (sh->last_status = exec_builtin(cmd, &sh->env));
	}
	pid = fork();
	if (pid < 0)
		return (perror("fork"), close_hdocs_in_cmd(cmd), sh->last_status = 1);
	if (pid == 0)
		exec_child_single(cmd, sh);
	wait_child_and_set_status(pid, sh);
	close_hdocs_in_cmd(cmd);
	return (sh->last_status);
}
