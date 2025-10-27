/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hdoc_sig.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 13:08:44 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/27 14:51:15 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "signals.h"

int	read_heredoc_into_fd(int wfd, t_redir *r, t_shell *sh)
{
	int	rc;

	sig_setup_readline();
	g_sig = 0;
	rc = heredoc_loop(wfd, r, sh);
	sig_setup_readline();
	if (g_sig)
	{
		g_sig = 0;
		sh->last_status = 130;
		return (130);
	}
	if (rc != 0)
	{
		sh->last_status = 1;
		return (1);
	}
	return (0);
}
