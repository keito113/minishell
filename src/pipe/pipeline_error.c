/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_error.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 16:35:11 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/22 19:51:58 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	pipe_cmds_cleanup_and_return(t_cmd **pipeline_cmds, t_shell *sh)
{
	free(pipeline_cmds);
	return (sh->last_status);
}

int	pipe_error_cleanup(t_pipe_ctx *ctx, t_cmd **pipeline_cmds, size_t n,
		t_shell *sh)
{
	if (ctx->prev_read != -1)
		close(ctx->prev_read);
	if (ctx->need_pipe_out)
	{
		close(ctx->pipefd[0]);
		close(ctx->pipefd[1]);
	}
	close_hdocs_in_pipeline(pipeline_cmds, n);
	return (pipe_cmds_cleanup_and_return(pipeline_cmds, sh));
}
