/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 11:58:28 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/01 14:20:47 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exec_builtin(t_cmd *cmd, t_env **penv)
{
	char	**argv;
	int		status;

	status = 0;
	argv = cmd->argv;
	if (ft_strcmp(argv[0], "export") == 0)
		status = builtin_export(argv, penv);
	else if (ft_strcmp(argv[0], "exit") == 0)
		status = builtin_exit(argv, cmd->sh, cmd->is_child);
	else if (ft_strcmp(argv[0], "unset") == 0)
		status = builtin_unset(argv, penv);
	else if (ft_strcmp(argv[0], "env") == 0)
		status = builtin_env(argv, penv);
	// else if (ft_strcmp(argv[0], "cd") == 0)
	// 	status = builtin_cd(argv);
	// else if (ft_strcmp(argv[0], "echo") == 0)
	// 	status = builtin_echo(argv);
	else if (ft_strcmp(argv[0], "pwd") == 0)
		status = builtin_pwd(argv);
	return (status);
}
