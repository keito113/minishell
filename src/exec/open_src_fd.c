/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_src_fd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:37:54 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/22 19:52:53 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	open_file_with_mode(const char *path, int flags, int mode)
{
	int	fd;

	fd = open(path, flags, mode);
	if (fd < 0)
	{
		perror(path);
		return (-1);
	}
	return (fd);
}

int	open_src_fd(t_redir *redir)
{
	if (redir->kind == R_HDOC)
	{
		if (redir->hdoc_fd >= 0)
			return (redir->hdoc_fd);
		else
			return (-1);
	}
	else if (redir->kind == R_IN)
		return (open_file_with_mode(redir->arg, O_RDONLY, 0));
	else if (redir->kind == R_OUT)
		return (open_file_with_mode(redir->arg, O_CREAT | O_WRONLY | O_TRUNC,
				0644));
	else if (redir->kind == R_APP)
		return (open_file_with_mode(redir->arg, O_CREAT | O_WRONLY | O_APPEND,
				0644));
	return (-1);
}
