/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dup_word_info.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 15:19:43 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/22 19:37:54 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_wordinfo_parts(t_parts *parts, size_t filled)
{
	while (filled > 0)
	{
		free(parts[filled - 1].text);
		filled--;
	}
	free(parts);
}

static int	copy_part(t_parts *dst, const t_parts *src)
{
	dst->quote = src->quote;
	dst->text = NULL;
	if (src->text == NULL)
		return (0);
	dst->text = ft_strdup(src->text);
	if (dst->text == NULL)
		return (-1);
	return (0);
}

static void	init_wordinfo_header(t_wordinfo *dst, const t_wordinfo *src)
{
	dst->parts = NULL;
	dst->parts_count = src->parts_count;
	dst->had_dollar = src->had_dollar;
	dst->had_quotes = src->had_quotes;
}

t_wordinfo	*dup_wordinfo(const t_wordinfo *src)
{
	t_wordinfo	*dup;
	size_t		i;

	if (!src)
		return (NULL);
	dup = malloc(sizeof(*dup));
	if (!dup)
		return (NULL);
	init_wordinfo_header(dup, src);
	if (src->parts_count == 0)
		return (dup);
	dup->parts = malloc(sizeof(t_parts) * src->parts_count);
	if (!dup->parts)
		return (free(dup), NULL);
	i = 0;
	while (i < src->parts_count)
	{
		if (copy_part(&dup->parts[i], &src->parts[i]) != 0)
			return (free_wordinfo_parts(dup->parts, i), free(dup), NULL);
		i++;
	}
	return (dup);
}
