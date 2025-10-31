/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_export_arg.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 17:03:10 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/29 16:47:19 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_valid_identifier(const char *s)
{
	size_t	i;

	if (s == NULL)
		return (0);
	if (!(ft_isalpha((unsigned char)*s) || *s == '_'))
		return (0);
	i = 1;
	while (s[i] != '\0')
	{
		if (!(ft_isalnum((unsigned char)s[i]) || s[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

static int	parse_assign(const char *arg, char **key, char **val,
		int *is_append)
{
	const char	*eq;
	size_t		key_len;

	*key = NULL;
	*val = NULL;
	*is_append = 0;
	eq = ft_strchr(arg, '=');
	if (!eq)
	{
		*key = ft_strdup(arg);
		return (*key != NULL);
	}
	key_len = (size_t)(eq - arg);
	if (key_len >= 1 && arg[key_len - 1] == '+')
	{
		*is_append = 1;
		key_len--;
	}
	*key = ft_substr(arg, 0, (unsigned int)key_len);
	if (!*key || !is_valid_identifier(*key))
		return (free(*key), *key = NULL, 0);
	*val = ft_strdup(eq + 1);
	if (!*val)
		return (free(*key), *key = NULL, 0);
	return (1);
}

static int	report_invalid_identifier(char *arg, char *key, char *val)
{
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
	free(key);
	free(val);
	return (1);
}

int	handle_export_arg(char *arg, t_env **penv)
{
	char	*key;
	char	*val;
	int		is_append;
	t_env	*env;

	if (!parse_assign(arg, &key, &val, &is_append))
		return (report_invalid_identifier(arg, key, val));
	if (val == NULL)
	{
		env = env_find(*penv, key);
		if (env)
			env->exported = 1;
		else
			env_set(penv, key, "", 1);
	}
	else if (is_append)
	{
		if (env_append_value(penv, key, val) != 0)
			return (free(key), free(val), 1);
	}
	else if (env_set(penv, key, val, 1) != 0)
		return (free(key), free(val), 1);
	free(key);
	free(val);
	return (0);
}
