/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_pipeline.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 14:24:15 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/03 15:48:22 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "signals.h"

static int	setup_pipeline_exec(const t_ast *root, t_cmd ***out_seq,
		size_t *out_n, t_shell *sh)
{
	t_cmd	**pipe_cmds;
	size_t	count_cmds;

	pipe_cmds = NULL;
	count_cmds = 0;
	*out_seq = NULL;
	*out_n = 0;
	if (!root)
		return (return_laststatus(sh, 0));
	if (build_pipeline_cmds(root, &pipe_cmds, &count_cmds, sh) != 0)
		return (sh->last_status);
	if (prepare_all_heredocs(pipe_cmds, count_cmds, sh) != 0)
		return (pipe_cmds_cleanup_and_return(pipe_cmds, sh));
	*out_seq = pipe_cmds;
	*out_n = count_cmds;
	return (0);
}

static int	run_pipeline_loop(t_cmd **pipe_cmds, size_t count_cmds,
		t_pipe_ctx *pipe_ctx, t_shell *sh)
{
	pid_t	pid;
	size_t	i;

	i = 0;
	while (i < count_cmds)
	{
		if (pipe_ctx_prepare_step(pipe_ctx, i, count_cmds) < 0)
		{
			perror("pipe");
			return (pipe_error_cleanup(pipe_ctx, pipe_cmds, count_cmds, sh));
		}
		pid = fork();
		if (pid < 0)
		{
			perror("fork");
			return (pipe_error_cleanup(pipe_ctx, pipe_cmds, count_cmds, sh));
		}
		if (pid == 0)
			run_pipeline_child(pipe_ctx, pipe_cmds, i, sh);
		pipe_ctx->last_pid = pid;
		pipe_ctx_parent_after_fork(pipe_ctx);
		i++;
	}
	return (0);
}

int	run_pipeline(const t_ast *root, t_shell *sh)
{
	t_cmd		**pipe_cmds;
	size_t		n;
	t_pipe_ctx	pipe_ctx;

	pipe_cmds = NULL;
	n = 0;
	if (setup_pipeline_exec(root, &pipe_cmds, &n, sh) != 0)
		return (sh->last_status);
	pipe_ctx_init(&pipe_ctx);
	sig_setup_parent_wait();
	if (run_pipeline_loop(pipe_cmds, n, &pipe_ctx, sh) != 0)
	{
		sig_setup_readline();
		return (sh->last_status);
	}
	close_hdocs_in_pipeline(pipe_cmds, n);
	if (reap_pipeline_and_set_last_status(pipe_ctx.last_pid, sh) < 0)
		sh->last_status = 1;
	sig_setup_readline();
	free(pipe_cmds);
	return (sh->last_status);
}
