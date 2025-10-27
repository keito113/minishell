/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 12:18:44 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/27 12:24:27 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_env(char **argv, t_env **env)
{
	t_env	*cur;

	(void)argv;
	cur = *env;
	while (cur)
	{
		if (cur->exported && cur->val)
			ft_printf("%s=%s\n", cur->key, cur->val);
		cur = cur->next;
	}
	return (0);
}
