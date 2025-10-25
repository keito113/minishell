/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_builtin.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 16:37:12 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/24 17:28:56 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_export(char **argv, t_env **penv)
{
	int	i;
	int	status;

	status = 0;
	if (argv[1] == NULL)
	{
		print_export_sorted(*penv);
		return (0);
	}
	i = 1;
	while (argv[i] != NULL)
	{
		if (handle_export_arg(argv[i], penv) != 0)
			status = 1;
		i++;
	}
	return (status);
}
