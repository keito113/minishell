/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_export_arg.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 17:03:10 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/24 17:40:13 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_valid_identifier(const char *s)
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
	const char	*p;
	size_t		klen;

	*is_append = 0;
	*val = NULL;
	p = ft_strchr(arg, '=');
	if (p == NULL)
	{
		*key = ft_strdup(arg);
		return (*key != NULL);
	}
	klen = (size_t)(p - arg);
	if (klen >= 1 && arg[klen - 1] == '+')
	{
		*is_append = 1;
		klen -= 1;
	}
	*key = ft_substr(arg, 0, (unsigned int)klen);
	*val = ft_strdup(p + 1);
	if (*key == NULL || *val == NULL)
		return (free(*key), free(*val), 0);
	return (1);
}

int	handle_export_arg(char *arg, t_env **penv)
{
	char	*key;
	char	*val;
	int		is_append;
	t_env	*e;

	key = NULL;
	val = NULL;
	is_append = 0;
	if (!parse_assign(arg, &key, &val, &is_append) || !is_valid_identifier(key))
	{
		write(2, "minishell: export: `", 21);
		write(2, arg, ft_strlen(arg));
		write(2, "': not a valid identifier\n", 26);
		free(key);
		free(val);
		return (1);
	}
	if (val == NULL)
	{
		e = env_find(*penv, key);
		if (e)
			e->exported = 1;
		else
			env_set(penv, key, "", 1);
	}
	else if (is_append)
		env_append(penv, key, val);
	else
		env_set(penv, key, val, 1);
	free(key);
	free(val);
	return (0);
}
