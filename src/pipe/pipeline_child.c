/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_child.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 14:21:40 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/03 18:57:38 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	pipe_ctx_child_wire_stdio(const t_pipe_ctx *ctx)
{
	if (ctx->prev_read != -1)
	{
		if (dup2(ctx->prev_read, STDIN_FILENO) < 0)
			exit(1);
	}
	if (ctx->need_pipe_out)
	{
		if (dup2(ctx->pipefd[1], STDOUT_FILENO) < 0)
			exit(1);
	}
}

static void	pipe_ctx_child_close_unused(const t_pipe_ctx *ctx)
{
	if (ctx->prev_read != -1)
		close(ctx->prev_read);
	if (ctx->need_pipe_out)
	{
		close(ctx->pipefd[0]);
		close(ctx->pipefd[1]);
	}
}

void	run_pipeline_child(t_pipe_ctx *pipe_ctx, t_cmd **pipeline_cmds,
		size_t i, t_shell *sh)
{
	pipe_ctx_child_wire_stdio(pipe_ctx);   // STDIN/STDOUT をパイプに接続
	pipe_ctx_child_close_unused(pipe_ctx); // 子で不要端を閉じる
	if (apply_redirs(pipeline_cmds[i]) < 0)
		exit(1);
	// if (pipeline_cmds[i]->is_builtin)
	// {
	// 	int st = run_builtin(pipeline_cmds[i], sh);
	// 	exit(st & 0xFF);
	// }
	exec_external(pipeline_cmds[i]->argv, sh->envp);
	if (errno == ENOENT)
		exit(127);
	exit(126);
}
