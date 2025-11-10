/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig_setup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 12:50:38 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/10 08:21:53 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signals.h"

volatile sig_atomic_t	g_sig;

static void	handler_int_rl(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "^C", 2);
	g_sig = 1;
}

static int	rl_sigint_hook(void)
{
	if (g_sig)
	{
		rl_replace_line("", 0);
		rl_done = 1;
	}
	return (0);
}

void	sig_setup_readline(void)
{
	struct sigaction	sa;
	struct sigaction	ign;

	rl_catch_signals = 0;
	rl_event_hook = rl_sigint_hook;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = handler_int_rl;
	sigaction(SIGINT, &sa, NULL);
	sigemptyset(&ign.sa_mask);
	ign.sa_flags = 0;
	ign.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &ign, NULL);
}

void	sig_setup_parent_wait(void)
{
	struct sigaction	ign;

	sigemptyset(&ign.sa_mask);
	ign.sa_flags = 0;
	ign.sa_handler = SIG_IGN;
	sigaction(SIGINT, &ign, NULL);
	sigaction(SIGQUIT, &ign, NULL);
}

void	sig_setup_child_exec(void)
{
	struct sigaction	dfl;

	sigemptyset(&dfl.sa_mask);
	dfl.sa_flags = 0;
	dfl.sa_handler = SIG_DFL;
	sigaction(SIGINT, &dfl, NULL);
	sigaction(SIGQUIT, &dfl, NULL);
}
