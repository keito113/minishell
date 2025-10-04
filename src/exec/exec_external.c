/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:05:28 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/04 14:20:06 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	validate_and_exec(const char *path, char *const argv[],
		char *const envp[])
{
	if (access(path, F_OK) != 0)
		exit(127);
	if (access(path, X_OK) != 0)
		exit(126);
	execve(path, argv, envp);
	if (errno == ENOENT)
		exit(127);
	else
		exit(126);
}

void	exec_external(char *const argv[], char *const envp[])
{
	char	*path;

	if (!argv || !argv[0])
		exit(0);
	path = find_cmd_path(argv[0], envp);
	if (!path)
		exit(127);
	validate_and_exec(path, argv, envp);
}
