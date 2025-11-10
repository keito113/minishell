/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_prompt_loop.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 09:07:25 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/10 09:10:54 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_sigint_line(t_shell *sh, char *line)
{
	g_sig = 0;
	sh->last_status = 130;
	free(line);
	return (1);
}

static void	finalize_and_exit(t_shell *sh)
{
	int	st;

	st = sh->exit_status & 255;
	if (sh->env)
		free_env_list(&sh->env);
	rl_clear_history();
	exit(st);
}

static void	process_line(char *line, t_shell *sh)
{
	if (line[0] != '\0')
		add_history(line);
	handle_line(line, sh);
	free(line);
	if (sh->should_exit)
		finalize_and_exit(sh);
}

int	run_prompt_loop(t_shell *sh)
{
	char	*line;

	while (1)
	{
		sig_setup_readline();
		line = ms_readline("minishell$ ");
		if (g_sig)
		{
			if (!handle_sigint_line(sh, line))
				return (0);
			continue ;
		}
		if (!line)
		{
			write(STDOUT_FILENO, "exit\n", 5);
			return (0);
		}
		process_line(line, sh);
	}
}
