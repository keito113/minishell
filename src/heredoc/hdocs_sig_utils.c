/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hdocs_sig_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 12:19:35 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/07 12:20:40 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "signals.h"

static int	write_all(int fd, const void *buf, size_t len)
{
	const char	*p;
	ssize_t		w;

	p = (const char *)buf;
	while (len)
	{
		w = write(fd, p, len);
		if (w < 0)
		{
			if (errno == EINTR)
				continue ;
			return (1);
		}
		p += w;
		len -= (size_t)w;
	}
	return (0);
}

static int	heredoc_put_line(int fd, const char *line)
{
	size_t	len;

	if (!line)
		return (1);
	len = ft_strlen(line);
	if (len && write_all(fd, line, len))
		return (1);
	if (write_all(fd, "\n", 1))
		return (1);
	return (0);
}

static char	*heredoc_expand_line(const char *line, const t_redir *r,
		t_shell *sh)
{
	if (r && r->here_doc_quoted)
		return (ft_strdup(line));
	return (expand_chunk(line, sh));
}

int	heredoc_handle_line(int wfd, t_redir *r, t_shell *sh, char *line)
{
	char	*out;

	out = heredoc_expand_line(line, r, sh);
	if (!out)
		return (1);
	if (heredoc_put_line(wfd, out))
	{
		free(out);
		return (1);
	}
	free(out);
	return (0);
}

int	hdoc_line_matches(const char *line, const char *limiter)
{
	size_t	ln;
	size_t	lm;

	if (!line || !limiter)
		return (0);
	ln = ft_strlen(line);
	lm = ft_strlen(limiter);
	if (ln != lm)
		return (0);
	if (ft_strncmp(line, limiter, lm) == 0)
		return (1);
	return (0);
}
