/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   normalize_absolute_path.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 14:48:31 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/09 15:36:00 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static const char	*skip_separators(const char *pos)
{
	while (*pos == '/')
		pos++;
	return (pos);
}

static const char	*extract_segment(const char *pos, size_t *len)
{
	const char	*start;

	start = pos;
	while (*pos && *pos != '/')
		pos++;
	*len = (size_t)(pos - start);
	return (start);
}

static int	process_segment(t_pathvec *vec, const char *seg, size_t len)
{
	if (len == 0 || (len == 1 && seg[0] == '.'))
		return (0);
	if (len == 2 && seg[0] == '.' && seg[1] == '.')
	{
		if (vec->size > 0)
			path_vec_pop(vec);
		return (0);
	}
	if (path_vec_push_slice(vec, seg, len) != 0)
		return (-1);
	return (0);
}

char	*normalize_absolute_path(const char *path)
{
	t_pathvec	vec;
	const char	*pos;
	const char	*seg;
	size_t		len;
	char		*result;

	ft_bzero(&vec, sizeof(vec));
	pos = skip_separators(path);
	while (*pos)
	{
		seg = extract_segment(pos, &len);
		if (process_segment(&vec, seg, len) != 0)
		{
			path_vec_free(&vec);
			return (NULL);
		}
		pos = skip_separators(seg + len);
	}
	result = path_vec_to_absolute(&vec);
	path_vec_free(&vec);
	return (result);
}
