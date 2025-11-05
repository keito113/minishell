/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_single_command.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:06:06 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/05 13:37:23 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "signals.h"

static void	exec_child_single(t_cmd *cmd, t_shell *sh)
{
	sig_setup_child_exec();
	if (apply_redirs(cmd) < 0)
		exit(1);
	exec_external(cmd->argv, sh);
	exit(127);
}

static int	wait_child_and_set_status(pid_t pid, t_shell *sh)
{
	int	status;

	while (waitpid(pid, &status, 0) < 0)
	{
		if (errno == EINTR)
			continue ;
		perror("waitpid");
		sh->last_status = 1;
		return (sh->last_status);
	}
	sh->last_status = status_to_exitcode(status);
	return (sh->last_status);
}

static int	pre_single_command(t_cmd *cmd, t_shell *sh)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		sh->last_status = 1;
		return (1);
	}
	if (prepare_cmd_heredocs(cmd, sh, NULL) != 0)
	{
		close_hdocs_in_cmd(cmd);
		return (1);
	}
	return (0);
}

int	run_single_command(t_cmd *cmd, t_shell *sh)
{
	pid_t		pid;
	t_fd_backup	*backups;
	size_t		len;

	if (pre_single_command(cmd, sh))
		return (sh->last_status);
	if (cmd->is_builtin)
	{
		if (setup_builtin_redirects(cmd, &backups, &len) < 0)
			sh->last_status = 1;
		else
		{
			sh->last_status = exec_builtin(cmd, &sh->env);
			restore_builtin_redirects(backups, len);
		}
		close_hdocs_in_cmd(cmd);
		return (sh->last_status);
	}
	sig_setup_parent_wait();
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		sig_setup_readline();
		close_hdocs_in_cmd(cmd);
		sh->last_status = 1;
		return (sh->last_status);
	}
	if (pid == 0)
		exec_child_single(cmd, sh);
	wait_child_and_set_status(pid, sh);
	sig_setup_readline();
	close_hdocs_in_cmd(cmd);
	return (sh->last_status);
}
