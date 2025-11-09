/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_vec_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 14:51:22 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/09 15:54:28 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	path_vec_push(t_pathvec *vec, char *segment)
{
	char	**tmp;
	size_t	new_cap;

	if (vec->size == vec->cap)
	{
		if (vec->cap == 0)
			new_cap = 8;
		else
			new_cap = vec->cap * 2;
		tmp = malloc(sizeof(char *) * new_cap);
		if (!tmp)
			return (-1);
		if (vec->segments)
			ft_memcpy(tmp, vec->segments, sizeof(char *) * vec->size);
		free(vec->segments);
		vec->segments = tmp;
		vec->cap = new_cap;
	}
	vec->segments[vec->size++] = segment;
	return (0);
}

int	path_vec_push_slice(t_pathvec *vec, const char *start, size_t len)
{
	char	*segment;

	segment = malloc(len + 1);
	if (!segment)
		return (-1);
	ft_memcpy(segment, start, len);
	segment[len] = '\0';
	if (path_vec_push(vec, segment) != 0)
	{
		free(segment);
		return (-1);
	}
	return (0);
}

void	path_vec_pop(t_pathvec *vec)
{
	if (vec->size == 0)
		return ;
	free(vec->segments[vec->size - 1]);
	vec->size--;
}

void	path_vec_free(t_pathvec *vec)
{
	size_t	i;

	i = 0;
	while (i < vec->size)
		free(vec->segments[i++]);
	free(vec->segments);
	vec->segments = NULL;
	vec->size = 0;
	vec->cap = 0;
}
