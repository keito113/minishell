/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   restore_builtin_redirects.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 13:35:08 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/05 13:46:39 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	target_fd_from_redir(const t_redir *redir)
{
	if (redir->fd_target >= 0)
		return (redir->fd_target);
	if (redir->kind == R_IN || redir->kind == R_HDOC)
		return (STDIN_FILENO);
	return (STDOUT_FILENO);
}

static int	backup_target_fd(int target_fd, t_fd_backup *arr, size_t *len)
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

int	setup_builtin_redirects(t_cmd *cmd, t_fd_backup **backups, size_t *len)
{
	size_t	cap;
	t_redir	*tmp;
	size_t	i;

	cap = 0;
	tmp = cmd->redirs;
	while (tmp)
	{
		cap++;
		tmp = tmp->next;
	}
	if (cap == 0)
	{
		*backups = NULL;
		*len = 0;
		return (0);
	}
	*backups = malloc(sizeof(**backups) * cap);
	if (!*backups)
		return (perror("malloc"), -1);
	*len = 0;
	tmp = cmd->redirs;
	while (tmp)
	{
		if (backup_target_fd(target_fd_from_redir(tmp), *backups, len) < 0)
		{
			perror("dup");
			i = 0;
			while (i < *len)
			{
				if ((*backups)[i].saved_fd >= 0)
					close((*backups)[i].saved_fd);
				i++;
			}
			free(*backups);
			return (-1);
		}
		tmp = tmp->next;
	}
	if (apply_redirs(cmd) < 0)
	{
		i = 0;
		while (i < *len)
		{
			if ((*backups)[i].saved_fd >= 0)
				close((*backups)[i].saved_fd);
			else
				close((*backups)[i].target_fd);
			i++;
		}
		free(*backups);
		return (-1);
	}
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
