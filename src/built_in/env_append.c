/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_append.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 11:12:23 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/27 12:56:38 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	append_create_if_missing(t_env **head, const char *key,
		char *safe_suffix)
{
	if (env_set(head, key, safe_suffix, 1) == -1)
		return (free(safe_suffix), -1);
	free(safe_suffix);
	return (0);
}

static int	ensure_env_value(t_env *node)
{
	if (node->val)
		return (0);
	node->val = ft_strdup("");
	if (node->val == NULL)
		return (-1);
	return (0);
}

static int	replace_env_value(t_env *node, char *safe_suffix)
{
	char	*merged;

	merged = ft_strjoin(node->val, safe_suffix);
	if (merged == NULL)
		return (-1);
	free(node->val);
	node->val = merged;
	node->exported = 1;
	return (0);
}

int	env_append_value(t_env **head, const char *key, const char *suffix)
{
	t_env	*node;
	char	*safe_suffix;

	if (!head || !key)
		return (-1);
	node = env_find(*head, key);
	safe_suffix = dup_or_empty(suffix);
	if (safe_suffix == NULL)
		return (-1);
	if (node == NULL)
		return (append_create_if_missing(head, key, safe_suffix));
	if (ensure_env_value(node) == -1)
		return (free(safe_suffix), -1);
	if (replace_env_value(node, safe_suffix) == -1)
		return (free(safe_suffix), -1);
	free(safe_suffix);
	return (0);
}
