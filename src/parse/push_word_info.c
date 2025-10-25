/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_word_info.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 15:04:03 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/22 15:30:52 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/minishell.h"

void	free_wordinfo_dup(t_wordinfo *info)
{
	size_t	index;

	if (!info)
		return ;
	if (info->parts)
	{
		index = 0;
		while (index < info->parts_count)
			free(info->parts[index++].text);
		free(info->parts);
	}
	free(info);
}

int	push_word_info(t_cmd *cmd, const t_token *token)
{
	t_wordinfo	**new_infos;
	t_wordinfo	*copy;
	size_t		i;

	if (!cmd || !token || token->token_kind != TK_WORD)
		return (-1);
	copy = dup_wordinfo(&token->word_info);
	if (!copy)
		return (-1);
	new_infos = malloc(sizeof(*new_infos) * (cmd->argc + 1));
	if (!new_infos)
		return (free_wordinfo_dup(copy), -1);
	i = 0;
	while (i < cmd->argc)
	{
		new_infos[i] = cmd->word_infos[i];
		i++;
	}
	new_infos[cmd->argc] = copy;
	free(cmd->word_infos);
	cmd->word_infos = new_infos;
	cmd->argc += 1;
	return (0);
}
