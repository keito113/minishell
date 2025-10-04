/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 14:10:56 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/04 13:37:18 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	prepare_all_heredocs(t_cmd **pipeline_cmds, size_t count_cmd, t_shell *sh)
{
	size_t	index;

	index = 0;
	while (index < count_cmd)
	{
		if (prepare_cmd_heredocs(pipeline_cmds[index], sh, NULL) != 0)
			return (sh->last_status);
		index++;
	}
	return (0);
}

void	close_hdocs_in_pipeline(t_cmd **pipeline_cmds, size_t count_cmd)
{
	size_t	index;

	index = 0;
	while (index < count_cmd)
	{
		close_hdocs_in_cmd(pipeline_cmds[index]);
		index++;
	}
}
