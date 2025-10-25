/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 18:15:56 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/22 19:37:31 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_cmd_word_infos(t_cmd *cmd)
{
	size_t	i;

	if (!cmd->word_infos)
		return ;
	i = 0;
	while (i < cmd->argc)
	{
		free_wordinfo_dup(cmd->word_infos[i]);
		i++;
	}
	free(cmd->word_infos);
	cmd->word_infos = NULL;
	cmd->argc = 0;
}

static void	free_redirs(t_redir **head)
{
	t_redir	*next;

	while (*head)
	{
		next = (*head)->next;
		if ((*head)->word_info)
			free_wordinfo_dup((*head)->word_info);
		free((*head)->arg);
		free(*head);
		*head = next;
	}
}

static void	free_argv(char **argv)
{
	size_t	i;

	if (!argv)
		return ;
	i = 0;
	while (argv[i])
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}

void	free_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	free_argv(cmd->argv);
	cmd->argv = NULL;
	free_cmd_word_infos(cmd);
	free_redirs(&cmd->redirs);
}

t_ast	*parse_command_fail(t_cmd *cmd)
{
	free_cmd(cmd);
	return (NULL);
}
