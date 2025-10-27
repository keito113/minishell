/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:50:44 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/27 11:53:24 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	mark_builtins_after_expand(t_ast *node)
{
	if (node == NULL)
		return ;
	if (node->type == AST_CMD)
	{
		if (node->as.cmd.argv && node->as.cmd.argv[0])
			node->as.cmd.is_builtin = is_builtin_name(node->as.cmd.argv[0]);
		else
			node->as.cmd.is_builtin = 0;
		return ;
	}
	mark_builtins_after_expand(node->as.pipe.left);
	mark_builtins_after_expand(node->as.pipe.right);
}

int	exec_entry(t_ast *root, t_shell *sh)
{
	if (!root)
		return (sh->last_status);
	mark_builtins_after_expand(root);
	if (root->type == AST_CMD)
		return (run_single_command(&root->as.cmd, sh));
	return (run_pipeline(root, sh));
}
