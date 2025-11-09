/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_absolute_path.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 13:57:55 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/09 15:54:38 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*snapshot_current_dir(t_env **env)
{
	const t_env	*pwd_entry;
	char		*dup;

	pwd_entry = env_find(*env, "PWD");
	if (pwd_entry && pwd_entry->val)
		return (ft_strdup(pwd_entry->val));
	dup = getcwd(NULL, 0);
	if (!dup)
	{
		write(2, "minishell: cd: unable to resolve current directory: ", 52);
		write(2, strerror(errno), ft_strlen(strerror(errno)));
		write(2, "\n", 1);
	}
	return (dup);
}

static char	*join_base_relative(const char *base, const char *rel)
{
	size_t	len_base;
	size_t	len_rel;
	size_t	need_sep;
	char	*joined;

	len_base = ft_strlen(base);
	len_rel = ft_strlen(rel);
	if (len_rel == 0)
		return (ft_strdup(base));
	need_sep = (len_base > 0 && base[len_base - 1] != '/');
	joined = malloc(len_base + need_sep + len_rel + 1);
	if (!joined)
		return (NULL);
	ft_memcpy(joined, base, len_base);
	if (need_sep)
		joined[len_base++] = '/';
	ft_memcpy(joined + len_base, rel, len_rel);
	joined[len_base + len_rel] = '\0';
	return (joined);
}

char	*build_absolute_path(const char *path, t_env **env)
{
	char	*base;
	char	*joined;
	char	*normalized;

	if (!path || path[0] == '\0')
		return (snapshot_current_dir(env));
	if (path[0] == '/')
		return (normalize_absolute_path(path));
	base = snapshot_current_dir(env);
	if (!base)
		return (NULL);
	joined = join_base_relative(base, path);
	free(base);
	if (!joined)
		return (NULL);
	normalized = normalize_absolute_path(joined);
	free(joined);
	return (normalized);
}
