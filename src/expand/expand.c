/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:04:24 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/09 17:52:59 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
