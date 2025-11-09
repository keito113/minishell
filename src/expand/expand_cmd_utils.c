/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_cmd_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 17:50:15 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/09 19:49:56 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

const char	*get_ifs_value(t_shell *sh)
{
	const t_env	*ifs_entry;

	ifs_entry = env_find(sh->env, "IFS");
	if (ifs_entry && ifs_entry->val && ifs_entry->val[0])
		return (ifs_entry->val);
	return (" \t\n");
}

void	free_strarray(char **arr, ssize_t count)
{
	ssize_t	i;

	if (!arr)
		return ;
	if (count < 0)
	{
		i = 0;
		while (arr[i])
			free(arr[i++]);
	}
	else
	{
		i = 0;
		while (i < count)
		{
			free(arr[i]);
			i++;
		}
	}
	free(arr);
}

int	remove_arg(t_cmd *cmd, size_t index)
{
	size_t		i;
	t_wordinfo	*info;

	if (index >= cmd->argc)
		return (0);
	free(cmd->argv[index]);
	cmd->argv[index] = NULL;
	info = cmd->word_infos[index];
	if (info)
	{
		free_wordinfo_dup(info);
		cmd->word_infos[index] = NULL;
	}
	i = index;
	while (i + 1 < cmd->argc)
	{
		cmd->argv[i] = cmd->argv[i + 1];
		cmd->word_infos[i] = cmd->word_infos[i + 1];
		i++;
	}
	cmd->argc--;
	cmd->argv[cmd->argc] = NULL;
	cmd->word_infos[cmd->argc] = NULL;
	return (0);
}

size_t	field_count(char **fields)
{
	size_t	len;

	len = 0;
	while (fields && fields[len])
		len++;
	return (len);
}
