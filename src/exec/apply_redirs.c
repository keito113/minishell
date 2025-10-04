/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_redirs.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:02:16 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/04 14:37:49 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	inter_target_fd(const t_redir *redir)
{
	if (!redir)
		return (-1);
	if (redir->fd_target >= 0)
		return (redir->fd_target);
	if (redir->kind == R_IN || redir->kind == R_HDOC)
		return (STDIN_FILENO);
	return (STDOUT_FILENO);
}

static int	redirect_fd(int src_fd, int target_fd, int kind)
{
	if (src_fd != target_fd)
	{
		if (dup2(src_fd, target_fd) < 0)
		{
			perror("dup2");
			close(src_fd);
			return (-1);
		}
	}
	if (kind != R_HDOC || (kind == R_HDOC && src_fd != target_fd))
		close(src_fd);
	return (0);
}

static int	apply_one_redir(const t_redir *redir)
{
	int	src_fd;
	int	target_fd;

	src_fd = open_src_fd((t_redir *)redir);
	target_fd = inter_target_fd(redir);
	if (target_fd < 0 || target_fd >= 1024)
		return (perror("bad target fd"), -1);
	if (src_fd < 0)
		return (-1);
	return (redirect_fd(src_fd, target_fd, redir->kind));
}

int	apply_redirs(const t_cmd *cmd)
{
	t_redir	*redir;

	if (!cmd)
		return (-1);
	redir = cmd->redirs;
	while (redir)
	{
		if (apply_one_redir(redir) < 0)
			return (-1);
		redir = redir->next;
	}
	return (0);
}
