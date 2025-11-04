/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_buiitin_name.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:44:06 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/03 10:20:15 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_builtin_name(const char *name)
{
	if (name == NULL || *name == '\0')
		return (0);
	if (ft_strcmp(name, "echo") == 0)
		return (1);
	// if (ft_strcmp(name, "cd") == 0)
	// 	return (1);
	if (ft_strcmp(name, "pwd") == 0)
		return (1);
	if (ft_strcmp(name, "export") == 0)
		return (1);
	if (ft_strcmp(name, "unset") == 0)
		return (1);
	if (ft_strcmp(name, "env") == 0)
		return (1);
	if (ft_strcmp(name, "exit") == 0)
		return (1);
	return (0);
}
