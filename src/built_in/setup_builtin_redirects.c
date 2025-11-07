/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_builtin_redirects.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 14:58:54 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/05 15:04:19 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	redir_count(const t_redir *redir)
{
	size_t	count;

	count = 0;
	while (redir)
	{
		count++;
		redir = redir->next;
	}
	return (count);
}

static void	cleanup_backups(t_fd_backup *backups, size_t len,
		int close_target_when_unsaved)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		if (backups[i].saved_fd >= 0)
			close(backups[i].saved_fd);
		else if (close_target_when_unsaved)
			close(backups[i].target_fd);
		i++;
	}
}

static int	collect_backups(const t_redir *redir, t_fd_backup *backups,
		size_t *len)
{
	while (redir)
	{
		if (backup_target_fd(target_fd_from_redir(redir), backups, len) < 0)
			return (-1);
		redir = redir->next;
	}
	return (0);
}

static int	init_backups(t_cmd *cmd, size_t *cap, t_fd_backup **backups,
		size_t *len)
{
	*cap = redir_count(cmd->redirs);
	if (*cap == 0)
	{
		*backups = NULL;
		*len = 0;
		return (0);
	}
	*backups = malloc(sizeof(**backups) * *cap);
	if (!*backups)
		return (perror("malloc"), -1);
	*len = 0;
	if (collect_backups(cmd->redirs, *backups, len) < 0)
	{
		perror("dup");
		cleanup_backups(*backups, *len, 0);
		free(*backups);
		*backups = NULL;
		*cap = 0;
		return (-1);
	}
	return (1);
}

int	setup_builtin_redirects(t_cmd *cmd, t_fd_backup **backups, size_t *len)
{
	int		status;
	size_t	cap;

	status = init_backups(cmd, &cap, backups, len);
	if (status <= 0)
		return (status);
	if (apply_redirs(cmd) < 0)
	{
		cleanup_backups(*backups, *len, 1);
		free(*backups);
		*backups = NULL;
		return (-1);
	}
	return (0);
}
