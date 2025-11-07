/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   status.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 11:06:19 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/07 18:52:49 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "signals.h"

int	status_to_exitcode(int wstatus)
{
	int	sig;

	if (WIFEXITED(wstatus))
		return (WEXITSTATUS(wstatus));
	if (WIFSIGNALED(wstatus))
	{
		sig = WTERMSIG(wstatus);
		if (sig == SIGQUIT)
			write(2, "Quit: 3\n", 8);
		else if (sig == SIGINT)
			write(2, "\n", 1);
		return (128 + sig);
	}
	return (1);
}

int	return_laststatus(t_shell *sh, int error_code)
{
	sh->last_status = error_code;
	return (sh->last_status);
}
