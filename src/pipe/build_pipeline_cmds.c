/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_pipeline_cmds.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 18:48:41 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/22 19:47:25 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	count_cmds_in_pipeline(const t_ast *node)
{
	if (!node)
		return (0);
	if (node->type == AST_CMD)
		return (1);
	return (count_cmds_in_pipeline(node->as.pipe.left)
		+ count_cmds_in_pipeline(node->as.pipe.right));
}

static void	flatten_pipeline(const t_ast *node, t_cmd **pipeline_cmds,
		size_t *index)
{
	if (!node)
		return ;
	if (node->type == AST_PIPE)
	{
		flatten_pipeline(node->as.pipe.left, pipeline_cmds, index);
		flatten_pipeline(node->as.pipe.right, pipeline_cmds, index);
	}
	else
		pipeline_cmds[(*index)++] = (t_cmd *)&node->as.cmd;
}

int	build_pipeline_cmds(const t_ast *root, t_cmd ***out_seq, size_t *out_n,
		t_shell *sh)
{
	size_t	count_cmds;
	size_t	index;
	t_cmd	**pipeline_cmds;

	*out_seq = NULL;
	*out_n = 0;
	count_cmds = count_cmds_in_pipeline(root);
	if (count_cmds < 2)
		return (return_laststatus(sh, 2));
	pipeline_cmds = (t_cmd **)malloc(sizeof(t_cmd *) * count_cmds);
	if (!pipeline_cmds)
	{
		perror("malloc");
		return (return_laststatus(sh, 1));
	}
	index = 0;
	flatten_pipeline(root, pipeline_cmds, &index);
	*out_seq = pipeline_cmds;
	*out_n = count_cmds;
	return (0);
}
