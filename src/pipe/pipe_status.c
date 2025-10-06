/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_status.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 14:40:23 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/03 18:51:05 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	reap_pipeline_and_set_last_status(pid_t last_pid, t_shell *sh)
{
	int		wstatus;
	int		seen_last;
	pid_t	pid;

	seen_last = 0;
	while (1)
	{
		pid = waitpid(-1, &wstatus, 0);
		if (pid == -1)
		{
			if (errno == EINTR)
				continue ;
			if (errno == ECHILD)
				break ;
			return (-1);
		}
		if (pid == last_pid)
		{
			sh->last_status = status_to_exitcode(wstatus);
			seen_last = 1;
		}
	}
	if (!seen_last)
		return (-1);
	return (0);
}
