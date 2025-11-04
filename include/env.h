/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:29:43 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/04 13:29:56 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_H
# define ENV_H

# include "struct.h"

t_env	*env_find(t_env *head, const char *key);
int		env_set(t_env **head, const char *key, const char *value, int exported);
int		env_append_value(t_env **head, const char *key, const char *suffix);
char	*dup_or_empty(const char *value);
int		env_init_from_envp(t_env **head, char **envp);
void	free_env_list(t_env **head);
void	env_list_append_node(t_env **head, t_env *node);

#endif
