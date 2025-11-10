/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 12:28:33 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/10 09:15:35 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char **argv, char **envp)
{
	t_shell	sh;

	(void)argv;
	if (argc != 1)
	{
		write(STDERR_FILENO,
			"minishell: this program does not accept arguments\n", 50);
		return (1);
	}
	if (init_shell(&sh, envp) < 0)
	{
		write(STDERR_FILENO, "minishell: failed to init env\n", 30);
		return (1);
	}
	run_prompt_loop(&sh);
	if (sh.env)
		free_env_list(&sh.env);
	return (sh.last_status);
}
