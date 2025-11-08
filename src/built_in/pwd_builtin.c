/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 12:56:55 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/08 18:48:15 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*find_old_pwd(t_env *head, const char *key)
{
	while (head)
	{
		if (ft_strcmp(head->key, key) == 0)
			return (head->val);
		head = head->next;
	}
	return (NULL);
}

int	builtin_pwd(char **argv, t_env **penv)
{
	t_env	*cur;
	char	*cwd;

	(void)argv;
	cur = *penv;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		cwd = find_old_pwd(cur, "PWD");
		if (!cwd)
		{
			perror("minishell: pwd");
			return (1);
		}
	}
	ft_printf("%s\n", cwd);
	free(cwd);
	return (0);
}
