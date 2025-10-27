/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 14:35:26 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/26 14:37:31 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*env_find(t_env *head, const char *key)
{
	while (head)
	{
		if (ft_strcmp(head->key, key) == 0)
			return (head);
		head = head->next;
	}
	return (NULL);
}

int	env_set(t_env **head, const char *key, const char *value, int exported)
{
	t_env	*node;
	t_env	*new_node;
	char	*dup_key;
	char	*dup_val;
	const char *safe_val = value ? value : "";

	if (!head || !key)
		return (-1);
	node = env_find(*head, key);
	dup_val = ft_strdup(safe_val);
	if (!dup_val)
		return (-1);
	if (node)
	{
		free(node->val);
		node->val = dup_val;
		node->exported = exported;
		return (0);
	}
	dup_key = ft_strdup(key);
	new_node = malloc(sizeof(*new_node));
	if (!dup_key || !new_node)
		return (free(dup_key), free(dup_val), free(new_node), -1);
	new_node->key = dup_key;
	new_node->val = dup_val;
	new_node->exported = exported;
	new_node->next = *head;
	*head = new_node;
	return (0);
}

int	env_append(t_env **head, const char *key, const char *suffix)
{
	t_env	*node;
	char	*merged;
	const char *safe_suffix = suffix ? suffix : "";

	if (!head || !key)
		return (-1);
	node = env_find(*head, key);
	if (!node)
		return (env_set(head, key, safe_suffix, 1));
	if (!node->val)
		node->val = ft_strdup("");
	merged = ft_strjoin(node->val, safe_suffix);
	if (!merged)
		return (-1);
	free(node->val);
	node->val = merged;
	node->exported = 1;
	return (0);
}



