/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:04:24 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/08 16:56:33 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	expand_redir(t_redir *redir, t_shell *sh)
{
	char	*expanded;

	while (redir)
	{
		if (redir->word_info && redir->word_info->had_dollar
			&& !redir->here_doc_quoted)
		{
			expanded = expand_word(redir->word_info, sh);
			if (!expanded)
				return (-1);
			free(redir->arg);
			redir->arg = expanded;
		}
		redir = redir->next;
	}
	return (0);
}

static int	expand_cmd(t_cmd *cmd, t_shell *sh)
{
	size_t		index;
	t_wordinfo	*info;
	char		*expanded;

	if (!cmd)
		return (0);
	index = 0;
	while (index < cmd->argc)
	{
		info = cmd->word_infos[index];
		if (info && info->had_dollar)
		{
			expanded = expand_word(info, sh);
			if (!expanded)
				return (-1);
			free(cmd->argv[index]);
			cmd->argv[index] = expanded;
		}
		index++;
	}
	return (expand_redir(cmd->redirs, sh));
}

int	expand(t_ast *node, t_shell *sh)
{
	if (!node)
		return (0);
	if (node->type == AST_CMD)
		return (expand_cmd(&node->as.cmd, sh));
	if (expand(node->as.pipe.left, sh) == -1)
		return (-1);
	return (expand(node->as.pipe.right, sh));
}
