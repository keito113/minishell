/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 17:46:25 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/14 18:03:21 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_ast	*create_pipe_node(t_ast *left, t_ast *right)
{
	t_ast	*node;

	node = malloc(sizeof(*node));
	if (!node)
		return (NULL);
	node->type = AST_PIPE;
	node->as.pipe.left = left;
	node->as.pipe.right = right;
	return (node);
}

static t_ast	*append_pipe_segment(const t_token *tokens, size_t len,
		size_t *idx, t_ast *left, t_shell *sh)
{
	t_ast	*right;
	t_ast	*pipe;

	(*idx)++;
	right = parse_command(tokens, len, idx);
	if (!right)
	{
		if (sh)
			sh->last_status = 1;
		free_ast(left);
		return (NULL);
	}
	pipe = create_pipe_node(left, right);
	if (!pipe)
	{
		if (sh)
			sh->last_status = 1;
		free_ast(left);
		free_ast(right);
		return (NULL);
	}
	return (pipe);
}

static inline t_ast	*ret_status(t_shell *sh, int code)
{
	if (sh)
		sh->last_status = code;
	return (NULL);
}

t_ast	*make_ast(t_tokvec *tokenvec, t_shell *sh)
{
	size_t	index;
	t_ast	*left;

	if (!tokenvec || !tokenvec->vector || tokenvec->len == 0)
		return (NULL);
	if (!precheck_syntax(tokenvec, sh))
		return (ret_status(sh, 2));
	index = 0;
	left = parse_command(tokenvec->vector, tokenvec->len, &index);
	if (!left)
		return (ret_status(sh, 1));
	while (index < tokenvec->len
		&& tokenvec->vector[index].token_kind == TK_PIPE)
	{
		left = append_pipe_segment(tokenvec->vector, tokenvec->len, &index,
				left, sh);
		if (!left)
			return (NULL);
	}
	return (left);
}
