/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_pipeline.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 14:24:15 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/01 16:03:28 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	flatten_pipeline(const t_ast *node, t_cmd **out, size_t *i)
{
	if (!node)
		return ;
	if (node->type == AST_PIPE)
	{
		flatten_pipeline(node->as.pipe.left, out, i);
		flatten_pipeline(node->as.pipe.right, out, i);
	}
	else // AST_CMD
		out[(*i)++] = (t_cmd *)&node->as.cmd;
}
