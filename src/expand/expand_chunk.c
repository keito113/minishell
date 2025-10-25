/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_chunk.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 11:20:27 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/22 19:33:04 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

const char	*lookup_env(const char *name, t_shell *sh)
{
	t_env	*node;

	if (!name || !*name || !sh)
		return (NULL);
	node = sh->env;
	while (node)
	{
		if (ft_strcmp(node->key, name) == 0)
			return (node->val);
		node = node->next;
	}
	return (NULL);
}

static int	append_status_expansion(char **result, size_t *index, t_shell *sh)
{
	*result = join_num_and_free(*result, sh->last_status);
	if (!*result)
		return (-1);
	*index += 2;
	return (0);
}

static int	append_env_expansion(char **result, const char *chunk,
		size_t *index, t_shell *sh)
{
	size_t		len;
	char		*name;
	const char	*val;

	len = var_length(chunk + *index + 1);
	name = ft_substr(chunk, *index + 1, len);
	if (!name)
		return (-1);
	val = lookup_env(name, sh);
	free(name);
	if (!val)
		val = "";
	*result = join_and_free(*result, val);
	if (!*result)
		return (-1);
	*index += len + 1;
	return (0);
}

static int	append_next_segment(char **result, const char *chunk, size_t *index,
		t_shell *sh)
{
	char	buf[2];

	if (chunk[*index] == '$' && chunk[*index + 1] == '?')
		return (append_status_expansion(result, index, sh));
	else if (chunk[*index] == '$' && is_valid_var_head(chunk[*index + 1]))
		return (append_env_expansion(result, chunk, index, sh));
	else
	{
		buf[0] = chunk[*index];
		buf[1] = '\0';
		*result = join_and_free(*result, buf);
		if (!*result)
			return (-1);
		(*index)++;
	}
	return (0);
}

char	*expand_chunk(const char *chunk, t_shell *sh)
{
	size_t	index;
	char	*result;

	if (!chunk)
		return (ft_strdup(""));
	result = ft_strdup("");
	if (!result)
		return (NULL);
	index = 0;
	while (chunk[index])
	{
		if (append_next_segment(&result, chunk, &index, sh) < 0)
			return (NULL);
	}
	return (result);
}
