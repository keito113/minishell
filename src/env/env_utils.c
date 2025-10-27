/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 14:35:26 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/27 12:48:18 by takawagu         ###   ########.fr       */
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

char	*dup_or_empty(const char *value)
{
	if (value == NULL)
		return (ft_strdup(""));
	return (ft_strdup(value));
}

static t_env	*env_create_node(const char *key, char *dup_val, int exported)
{
	t_env	*new_node;
	char	*dup_key;

	dup_key = ft_strdup(key);
	if (dup_key == NULL)
		return (free(dup_val), NULL);
	new_node = malloc(sizeof(*new_node));
	if (new_node == NULL)
		return (free(dup_key), free(dup_val), NULL);
	new_node->key = dup_key;
	new_node->val = dup_val;
	new_node->exported = exported;
	new_node->next = NULL;
	return (new_node);
}

int	env_set(t_env **head, const char *key, const char *value, int exported)
{
	t_env	*node;
	t_env	*new_node;
	char	*dup_val;

	if (head == NULL || key == NULL)
		return (-1);
	node = env_find(*head, key);
	dup_val = dup_or_empty(value);
	if (dup_val == NULL)
		return (-1);
	if (node != NULL)
	{
		free(node->val);
		node->val = dup_val;
		node->exported = exported;
		return (0);
	}
	new_node = env_create_node(key, dup_val, exported);
	if (new_node == NULL)
		return (-1);
	env_list_append_node(head, new_node);
	return (0);
}
