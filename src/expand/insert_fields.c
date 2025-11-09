/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   insert_fields.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 17:45:45 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/09 18:02:13 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	init_insert_ctx(t_insert_ctx *ctx, t_cmd *cmd, size_t nf)
{
	ctx->cmd = cmd;
	ctx->nf = nf;
	ctx->new_argv = malloc(sizeof(char *) * (cmd->argc + nf));
	ctx->new_infos = malloc(sizeof(t_wordinfo *) * (cmd->argc + nf));
	if (!ctx->new_argv || !ctx->new_infos)
	{
		free(ctx->new_argv);
		free(ctx->new_infos);
		return (-1);
	}
	return (0);
}

static size_t	copy_prefix(t_insert_ctx *ctx, size_t index)
{
	size_t	i;

	i = 0;
	while (i < index)
	{
		ctx->new_argv[i] = ctx->cmd->argv[i];
		ctx->new_infos[i] = ctx->cmd->word_infos[i];
		i++;
	}
	return (i);
}

static size_t	insert_new_fields(t_insert_ctx *ctx, size_t index,
		char **fields)
{
	size_t	i;
	size_t	dst;

	dst = index;
	free(ctx->cmd->argv[index]);
	i = 0;
	while (i < ctx->nf)
	{
		ctx->new_argv[dst] = fields[i];
		fields[i] = NULL;
		if (i == 0)
			ctx->new_infos[dst] = ctx->cmd->word_infos[index];
		else
			ctx->new_infos[dst] = NULL;
		dst++;
		i++;
	}
	return (dst);
}

static void	copy_suffix(t_insert_ctx *ctx, size_t index, size_t dst)
{
	size_t	i;

	i = index + 1;
	while (i < ctx->cmd->argc)
	{
		ctx->new_argv[dst] = ctx->cmd->argv[i];
		ctx->new_infos[dst] = ctx->cmd->word_infos[i];
		dst++;
		i++;
	}
	ctx->new_argv[dst] = NULL;
	ctx->cmd->argc = dst;
}

int	insert_fields(t_cmd *cmd, size_t index, char **fields)
{
	t_insert_ctx	ctx;
	size_t			start;

	ctx.nf = field_count(fields);
	if (ctx.nf == 0)
		return (remove_arg(cmd, index));
	if (init_insert_ctx(&ctx, cmd, ctx.nf) < 0)
		return (-1);
	start = copy_prefix(&ctx, index);
	start = insert_new_fields(&ctx, index, fields);
	copy_suffix(&ctx, index, start);
	free(cmd->argv);
	free(cmd->word_infos);
	cmd->argv = ctx.new_argv;
	cmd->word_infos = ctx.new_infos;
	return (0);
}
