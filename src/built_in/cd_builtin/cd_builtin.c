/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_builtin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 12:39:21 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/09 15:23:55 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	update_pwd_vars(t_env **env, const char *logical_path)
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
	{
		if (!logical_path)
			if (env_set(env, "PWD", logical_path, 1) < 0)
				return (-1);
		return (0);
	}
	if (env_set(env, "PWD", cwd, 1) < 0)
		return (free(cwd), -1);
	return (free(cwd), 0);
}

static int	do_chdir(const char *path, const char *display_path)
{
	if (chdir(path) == -1)
	{
		write(2, "minishell: cd: ", 15);
		write(2, display_path, ft_strlen(display_path));
		write(2, ": ", 2);
		write(2, strerror(errno), ft_strlen(strerror(errno)));
		write(2, "\n", 1);
		return (1);
	}
	return (0);
}

static int	resolve_home_path(t_env **env, char **out_path)
{
	const t_env	*home_entry;
	const char	*home;

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
	*out_path = ft_strdup(home);
	if (!*out_path)
	{
		write(2, "minishell: cd: malloc failed\n", 29);
		return (1);
	}
	return (0);
}

static int	resolve_cd_target(char **argv, t_env **env, char **out_path)
{
	int	argc;

	argc = 0;
	while (argv[argc])
		argc++;
	if (argc > 2)
	{
		write(2, "minishell: cd: too many arguments\n", 34);
		return (1);
	}
	if (argc == 1)
		return (resolve_home_path(env, out_path));
	*out_path = ft_strdup(argv[1]);
	return (0);
}

int	builtin_cd(char **argv, t_env **env)
{
	char	*raw;
	char	*abs;

	if (resolve_cd_target(argv, env, &raw) != 0)
		return (1);
	abs = build_absolute_path(raw, env);
	if (!abs)
		return (1);
	if (do_chdir(abs, raw) != 0)
		return (free(raw), free(abs), 1);
	if (update_pwd_vars(env, abs) < 0)
		return (free(raw), free(abs), 1);
	free(raw);
	free(abs);
	return (0);
}
