/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_ast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 14:47:28 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/14 14:51:56 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_redirs(t_redir *r)
{
	t_redir	*n;

	while (r)
	{
		n = r->next;
		free(r->arg);
		free(r);
		r = n;
	}
}

static void	free_argv(char **v)
{
	int	i;

	i = 0;
	if (!v)
		return ;
	while (v[i])
	{
		free(v[i]);
		i++;
	}
	free(v);
}

void	free_ast(t_ast *node)
{
	if (!node)
		return ;
	if (node->type == AST_PIPE)
	{
		free_ast(node->as.pipe.left);
		free_ast(node->as.pipe.right);
		free(node);
	}
	else // AST_CMD
	{
		free_argv(node->as.cmd.argv);
		free_redirs(node->as.cmd.redirs);
		free(node);
	}
}