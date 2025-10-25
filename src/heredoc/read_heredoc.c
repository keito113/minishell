/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 18:27:37 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/22 19:36:13 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*heredoc_maybe_expand(const char *line, const t_redir *redir,
		t_shell *sh)
{
	if (!line)
		return (NULL);
	if (redir->here_doc_quoted)
		return (ft_strdup(line));
	return (expand_chunk(line, sh));
}

static int	heredoc_put_line(int fd, const char *line)
{
	size_t	len;

	if (!line)
		return (1);
	len = ft_strlen(line);
	if (len > 0 && write(fd, line, len) < 0)
		return (1);
	if (write(fd, "\n", 1) < 0)
		return (1);
	return (0);
}

int	read_heredoc_into_fd(int write_fd, t_redir *redir, t_shell *sh)
{
	const char	*limiter;
	char		*line;
	char		*expanded;

	limiter = redir->arg;
	while (1)
	{
		line = readline("heredoc> ");
		if (!line)
			break ;
		if (ft_strcmp(line, limiter) == 0)
		{
			free(line);
			break ;
		}
		expanded = heredoc_maybe_expand(line, redir, sh);
		free(line);
		if (!expanded)
			return (1);
		if (heredoc_put_line(write_fd, expanded) != 0)
			return (free(expanded), 1);
		free(expanded);
	}
	return (0);
}
