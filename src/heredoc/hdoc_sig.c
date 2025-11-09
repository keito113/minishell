/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hdoc_sig.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 13:08:44 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/09 16:38:50 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "signals.h"

int	heredoc_loop(int wfd, t_redir *r, t_shell *sh)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (g_sig)
		{
			g_sig = 0;
			if (line)
				free(line);
			sh->last_status = 130;
			return (130);
		}
		if (!line)
			return (0);
		if (hdoc_line_matches(line, r->arg))
			return (free(line), 0);
		if (heredoc_handle_line(wfd, r, sh, line))
			return (free(line), 1);
		free(line);
	}
}

int	read_heredoc_into_fd(int wfd, t_redir *r, t_shell *sh)
{
	int	rc;

	sig_setup_readline();
	g_sig = 0;
	rc = heredoc_loop(wfd, r, sh);
	sig_setup_readline();
	if (rc == 130)
		return (130);
	if (rc != 0)
	{
		sh->last_status = 1;
		return (1);
	}
	return (0);
}
