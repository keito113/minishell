/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_word_to_argv.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:02:11 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/14 14:49:40 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*x_strdup(const char *str)
{
	size_t	n;
	char	*ptr;

	if (!str)
		return (NULL);
	n = ft_strlen(str);
	ptr = (char *)malloc(n + 1);
	if (!ptr)
		return (NULL);
	ft_memcpy(ptr, str, n + 1);
	return (ptr);
}

static size_t	old_argv_length(char *const *old_argv)
{
	size_t	len;

	len = 0;
	if (old_argv)
		while (old_argv[len])
			len++;
	return (len);
}

static int	grow_argv_with_word(t_cmd *cmd, char *word, size_t old_len)
{
	char	**new_argv;
	size_t	index;

	new_argv = (char **)malloc(sizeof(char *) * (old_len + 2));
	if (!new_argv)
		return (-1);
	index = 0;
	while (index < old_len)
	{
		new_argv[index] = cmd->argv[index];
		index++;
	}
	new_argv[old_len] = word;
	new_argv[old_len + 1] = NULL;
	free(cmd->argv);
	cmd->argv = new_argv;
	return (0);
}

int	push_word_to_argv(t_cmd *cmd, const t_token *token)
{
	char	**old_argv;
	size_t	old_argv_len;
	char	*word;

	if (!cmd || !token || token->token_kind != TK_WORD)
		return (-1);
	if (token->args)
		word = x_strdup(token->args);
	else
		word = x_strdup("");
	if (!word)
		return (-1);
	old_argv = cmd->argv;
	old_argv_len = old_argv_length(old_argv);
	if (grow_argv_with_word(cmd, word, old_argv_len) != 0)
	{
		free(word);
		return (-1);
	}
	return (0);
}
