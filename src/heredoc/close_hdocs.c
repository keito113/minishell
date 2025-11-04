/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   close_hdocs.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:42:19 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/22 19:34:01 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	close_hdocs_in_cmd(t_cmd *cmd)
{
	t_redir	*redir;

	redir = cmd->redirs;
	while (redir)
	{
		if (redir->kind == R_HDOC && redir->hdoc_fd >= 0)
		{
			close(redir->hdoc_fd);
			redir->hdoc_fd = -1;
		}
		redir = redir->next;
	}
}

void	close_all_prepared_hdocs(t_ast *node)
{
	if (!node)
		return ;
	if (node->type == AST_CMD)
	{
		close_hdocs_in_cmd(&node->as.cmd);
		return ;
	}
	close_all_prepared_hdocs(node->as.pipe.left);
	close_all_prepared_hdocs(node->as.pipe.right);
}
