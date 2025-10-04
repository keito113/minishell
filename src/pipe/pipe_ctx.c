/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_ctx.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 13:40:45 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/04 13:35:40 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	pipe_ctx_init(t_pipe_ctx *ctx)
{
	ctx->prev_read = -1;
	ctx->pipefd[0] = -1;
	ctx->pipefd[1] = -1;
	ctx->need_pipe_out = 0;
	ctx->last_pid = -1;
}

int	pipe_ctx_prepare_step(t_pipe_ctx *ctx, size_t i, size_t count_cmds)
{
	if (i + 1 < count_cmds)
	{
		if (pipe(ctx->pipefd) < 0)
			return (-1);
		ctx->need_pipe_out = 1;
	}
	else
	{
		ctx->pipefd[0] = -1;
		ctx->pipefd[1] = -1;
		ctx->need_pipe_out = 0;
	}
	return (0);
}

void	pipe_ctx_parent_after_fork(t_pipe_ctx *ctx)
{
	if (ctx->prev_read != -1)
		close(ctx->prev_read);
	if (ctx->need_pipe_out)
	{
		close(ctx->pipefd[1]);
		ctx->prev_read = ctx->pipefd[0];
	}
	else
		ctx->prev_read = -1;
}
