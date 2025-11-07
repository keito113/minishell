/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   restore_builtin_redirects.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 13:35:08 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/05 15:26:31 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	target_fd_from_redir(const t_redir *redir)
{
	if (redir->fd_target >= 0)
		return (redir->fd_target);
	if (redir->kind == R_IN || redir->kind == R_HDOC)
		return (STDIN_FILENO);
	return (STDOUT_FILENO);
}

int	backup_target_fd(int target_fd, t_fd_backup *arr, size_t *len)
{
	size_t	i;

	i = 0;
	while (i < *len)
	{
		if (arr[i].target_fd == target_fd)
			return (0);
		i++;
	}
	arr[*len].target_fd = target_fd;
	arr[*len].saved_fd = dup(target_fd);
	if (arr[*len].saved_fd < 0)
		arr[*len].saved_fd = -1;
	(*len)++;
	return (0);
}

void	restore_builtin_redirects(t_fd_backup *backups, size_t len)
{
	size_t	i;

	if (!backups)
		return ;
	i = 0;
	while (i < len)
	{
		if (backups[i].saved_fd >= 0)
		{
			if (dup2(backups[i].saved_fd, backups[i].target_fd) < 0)
				perror("dup2");
			close(backups[i].saved_fd);
		}
		else
			close(backups[i].target_fd);
		i++;
	}
	free(backups);
}
