/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_vec_to_absolute.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 15:44:14 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/09 15:53:02 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	calculate_total_len(const t_pathvec *vec)
{
	size_t	total;
	size_t	i;

	total = 1;
	i = 0;
	while (i < vec->size)
	{
		total += ft_strlen(vec->segments[i]);
		if (i + 1 < vec->size)
			total += 1;
		i++;
	}
	return (total);
}

static void	build_path_string(char *dst, const t_pathvec *vec)
{
	size_t	i;
	size_t	pos;
	size_t	seg_len;

	dst[0] = '/';
	pos = 1;
	i = 0;
	while (i < vec->size)
	{
		seg_len = ft_strlen(vec->segments[i]);
		ft_memcpy(dst + pos, vec->segments[i], seg_len);
		pos += seg_len;
		if (i + 1 < vec->size)
			dst[pos++] = '/';
		i++;
	}
	dst[pos] = '\0';
}

char	*path_vec_to_absolute(const t_pathvec *vec)
{
	char	*res;
	size_t	total_len;

	if (vec->size == 0)
		return (ft_strdup("/"));
	total_len = calculate_total_len(vec);
	res = malloc(total_len + 1);
	if (!res)
		return (NULL);
	build_path_string(res, vec);
	return (res);
}
