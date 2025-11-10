/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:25:07 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/10 08:37:52 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPE_H
# define PIPE_H

# include "struct.h"

int		reap_pipeline_and_set_last_status(pid_t last_pid, t_shell *sh);
void	pipe_ctx_init(t_pipe_ctx *ctx);
int		pipe_ctx_prepare_step(t_pipe_ctx *ctx, size_t i, size_t count_cmds);
void	pipe_ctx_parent_after_fork(t_pipe_ctx *ctx);
int		prepare_all_heredocs(t_cmd **pipeline_cmds, size_t count_cmds,
			t_shell *sh);
void	close_hdocs_in_pipeline(t_cmd **pipeline_cmds, size_t count_cmds);
void	run_pipeline_child(t_pipe_ctx *pipe_ctx, t_cmd **pipeline_cmds,
			size_t i, t_shell *sh);
int		pipe_cmds_cleanup_and_return(t_cmd **pipeline_cmds, t_shell *sh);
int		pipe_error_cleanup(t_pipe_ctx *ctx, t_cmd **pipeline_cmds, size_t n,
			t_shell *sh);
int		build_pipeline_cmds(const t_ast *root, t_cmd ***out_seq, size_t *out_n,
			t_shell *sh);
int		run_pipeline(const t_ast *root, t_shell *sh);
void	validate_and_exec_pipe(char *path, char *const argv[], t_shell *sh);
void	exec_external_pipe(char *const argv[], t_shell *sh);

#endif
