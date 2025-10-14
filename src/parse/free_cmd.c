/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 18:15:56 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/14 15:18:19 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_cmd(t_cmd *cmd)
{
	t_redir	*next;
	size_t	i;

	if (!cmd)
		return ;
	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
		{
			free(cmd->argv[i]);
			i++;
		}
		free(cmd->argv);
		cmd->argv = NULL;
	}
	while (cmd->redirs)
	{
		next = cmd->redirs->next;
		free(cmd->redirs->arg);
		free(cmd->redirs);
		cmd->redirs = next;
	}
}

t_ast	*parse_command_fail(t_cmd *cmd)
{
	free_cmd(cmd);
	return (NULL);
}
