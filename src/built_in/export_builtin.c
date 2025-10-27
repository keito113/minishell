/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_builtin.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 16:37:12 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/27 15:10:03 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_export(char **argv, t_env **penv)
{
	int	index;
	int	status;

	status = 0;
	if (argv[1] == NULL)
	{
		print_export_sorted(*penv);
		return (0);
	}
	index = 1;
	while (argv[index] != NULL)
	{
		if (handle_export_arg(argv[index], penv) != 0)
			return (-1);
		index++;
	}
	return (status);
}
