/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 17:46:25 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/22 19:58:25 by takawagu         ###   ########.fr       */
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

static t_ast	*append_pipe_segment(t_parse_ctx *ctx, t_ast *left)
{
	t_ast	*right;
	t_ast	*pipe;

	ctx->index++;
	right = parse_command(ctx->tokens, ctx->len, &ctx->index);
	if (!right)
	{
		if (ctx->sh)
			ctx->sh->last_status = 1;
		return (free_ast(left), NULL);
	}
	pipe = create_pipe_node(left, right);
	if (!pipe)
	{
		if (ctx->sh)
			ctx->sh->last_status = 1;
		return (free_ast(left), free_ast(right), NULL);
	}
	return (pipe);
}

static t_ast	*ret_and_set_status(t_shell *sh, int status_code)
{
	if (sh)
		sh->last_status = status_code;
	return (NULL);
}

t_ast	*make_ast(t_tokvec *tokenvec, t_shell *sh)
{
	t_parse_ctx	ctx;
	t_ast		*left;

	if (!tokenvec || !tokenvec->vector || tokenvec->len == 0)
		return (NULL);
	if (!precheck_syntax(tokenvec, sh))
		return (ret_and_set_status(sh, 2));
	ctx.tokens = tokenvec->vector;
	ctx.len = tokenvec->len;
	ctx.index = 0;
	ctx.sh = sh;
	left = parse_command(ctx.tokens, ctx.len, &ctx.index);
	if (!left)
		return (ret_and_set_status(sh, 1));
	while (ctx.index < ctx.len && ctx.tokens[ctx.index].token_kind == TK_PIPE)
	{
		left = append_pipe_segment(&ctx, left);
		if (!left)
			return (NULL);
	}
	return (left);
}

t_ast	*parse(t_tokvec *tokenvec, t_shell *sh)
{
	t_ast	*root;

	root = make_ast(tokenvec, sh);
	tokvec_free(tokenvec);
	return (root);
}
