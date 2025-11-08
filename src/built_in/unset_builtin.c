/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_builtin.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 14:46:44 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/08 18:40:47 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	env_unset(t_env **penv, const char *name)
{
	t_env	*cur;
	t_env	*prev;

	if (!penv || !*penv || !name)
		return (0);
	prev = NULL;
	cur = *penv;
	while (cur)
	{
		if (ft_strcmp(cur->key, name) == 0)
		{
			if (prev)
				prev->next = cur->next;
			else
				*penv = cur->next;
			free(cur->key);
			free(cur->val);
			free(cur);
			return (1);
		}
		prev = cur;
		cur = cur->next;
	}
	return (0);
}

static void	put_unset_bad_ident(const char *arg)
{
	write(2, "minishell: unset: `", 19);
	if (arg)
		write(2, arg, ft_strlen(arg));
	write(2, "': not a valid identifier\n", 26);
}

int	builtin_unset(char **argv, t_env **penv)
{
	int		status;
	size_t	i;

	status = 0;
	if (!argv || !argv[0])
		return (0);
	i = 1;
	while (argv[i])
	{
		if (!is_valid_identifier(argv[i]))
		{
			put_unset_bad_ident(argv[i]);
			status = 1;
		}
		else
			env_unset(penv, argv[i]);
		i++;
	}
	return (status);
}
