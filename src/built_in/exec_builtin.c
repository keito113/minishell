/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 11:58:28 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/08 18:34:58 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "echo.h"
#include "minishell.h"
#include "signals.h"

static int	dispatch_builtin(t_cmd *cmd, t_env **penv)
{
	char	**argv;
	int		status;

	argv = cmd->argv;
	status = 0;
	if (ft_strcmp(argv[0], "export") == 0)
		status = builtin_export(argv, penv);
	else if (ft_strcmp(argv[0], "exit") == 0)
		status = builtin_exit(argv, cmd->sh, cmd->is_child);
	else if (ft_strcmp(argv[0], "unset") == 0)
		status = builtin_unset(argv, penv);
	else if (ft_strcmp(argv[0], "env") == 0)
		status = builtin_env(argv, penv);
	else if (ft_strcmp(argv[0], "cd") == 0)
		status = builtin_cd(argv, penv);
	else if (ft_strcmp(argv[0], "echo") == 0)
		status = builtin_echo(argv, STDOUT_FILENO);
	else if (ft_strcmp(argv[0], "pwd") == 0)
		status = builtin_pwd(argv,penv);
	return (status);
}

int	exec_builtin(t_cmd *cmd, t_env **penv)
{
	int	status;

	status = 0;
	if (g_sig)
	{
		g_sig = 0;
		return (130);
	}
	status = dispatch_builtin(cmd, penv);
	if (g_sig)
	{
		g_sig = 0;
		return (130);
	}
	return (status);
}
