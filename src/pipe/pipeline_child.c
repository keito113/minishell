/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_child.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 14:21:40 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/03 15:37:51 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "signals.h"

static void	pipe_ctx_attach_child_stdio(const t_pipe_ctx *ctx)
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

static int	run_builtin(t_cmd *cmd, t_shell *sh)
{
	int	status;

	if (cmd == NULL || cmd->argv == NULL || cmd->argv[0] == NULL)
		return (1);
	status = exec_builtin(cmd, &sh->env);
	return (status);
}

void	run_pipeline_child(t_pipe_ctx *pipe_ctx, t_cmd **pipeline_cmds,
		size_t i, t_shell *sh)
{
	int	st;

	sig_setup_child_exec();
	pipe_ctx_attach_child_stdio(pipe_ctx);
	pipe_ctx_child_close_unused(pipe_ctx);
	if (apply_redirs(pipeline_cmds[i]) < 0)
		exit(1);
	if (pipeline_cmds[i]->is_builtin)
	{
		st = run_builtin(pipeline_cmds[i], sh);
		exit(st);
	}
	exec_external(pipeline_cmds[i]->argv, sh);
	exit(127);
}
