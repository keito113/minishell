/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child_single.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 14:00:43 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/07 16:08:37 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	cleanup_child_and_exit(t_shell *sh, int status)
{
	if (sh && sh->env)
		free_env_list(&sh->env);
	rl_clear_history();
	exit(status);
}

void	exec_child_single(t_cmd *cmd, t_shell *sh)
{
	sig_setup_child_exec();
	if (apply_redirs(cmd) < 0)
		cleanup_child_and_exit(sh, 1);
	exec_external(cmd->argv, sh);
	cleanup_child_and_exit(sh, 127);
}
