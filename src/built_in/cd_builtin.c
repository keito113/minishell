/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_builtin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 12:39:21 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/04 10:37:50 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	update_pwd_vars(t_env **env)
{
	char		*cwd;
	const t_env	*pwd_entry;
	const char	*old;

	pwd_entry = env_find(*env, "PWD");
	if (pwd_entry != NULL)
		old = pwd_entry->val;
	else
		old = NULL;
	if (old && env_set(env, "OLDPWD", old, 1) < 0)
		return (-1);
	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (0);
	if (env_set(env, "PWD", cwd, 1) < 0)
	{
		free(cwd);
		return (-1);
	}
	free(cwd);
	return (0);
}

static int	do_chdir(const char *path)
{
	if (chdir(path) == -1)
	{
		write(2, "minishell: cd: ", 15);
		write(2, path, ft_strlen(path));
		write(2, ": ", 2);
		write(2, strerror(errno), ft_strlen(strerror(errno)));
		write(2, "\n", 1);
		return (1);
	}
	return (0);
}

int	builtin_cd(char **argv, t_env **env)
{
	int			argc;
	char		*path;
	const t_env	*home_entry;
	const char	*home;

	argc = 0;
	while (argv[argc])
		argc++;
	if (argc > 2)
	{
		write(2, "minishell: cd: too many arguments\n", 34);
		return (1);
	}
	if (argc == 1)
	{
		home_entry = env_find(*env, "HOME");
		if (home_entry != NULL)
			home = home_entry->val;
		else
			home = NULL;
		if (!home || !*home)
		{
			write(2, "minishell: cd: HOME not set\n", 28);
			return (1);
		}
		path = (char *)home;
	}
	else
		path = argv[1];
	if (do_chdir(path) != 0)
		return (1);
	if (update_pwd_vars(env) < 0)
	{
		write(2, "minishell: cd: failed to update PWD/OLDPWD\n", 44);
		return (1);
	}
	return (0);
}
