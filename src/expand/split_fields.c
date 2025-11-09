/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_fields.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 16:52:45 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/09 17:39:56 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_ifs_char(char c, const char *ifs)
{
	size_t	i;

	if (!ifs || !*ifs)
		ifs = " \t\n";
	i = 0;
	while (ifs[i])
	{
		if (ifs[i] == c)
			return (1);
		i++;
	}
	return (0);
}

static size_t	count_fields(const char *s, const char *ifs)
{
	size_t	count;
	size_t	i;

	count = 0;
	i = 0;
	while (s && s[i])
	{
		while (s[i] && is_ifs_char(s[i], ifs))
			i++;
		if (!s[i])
			break ;
		count++;
		while (s[i] && !is_ifs_char(s[i], ifs))
			i++;
	}
	return (count);
}

static int	extract_next_field(const char *s, const char *ifs, size_t *pos,
		char **out_slot)
{
	size_t	start;
	size_t	i;

	i = *pos;
	while (s[i] && is_ifs_char(s[i], ifs))
		i++;
	if (!s[i])
	{
		*pos = i;
		return (0);
	}
	start = i;
	while (s[i] && !is_ifs_char(s[i], ifs))
		i++;
	*out_slot = ft_substr(s, start, i - start);
	if (!*out_slot)
		return (-1);
	*pos = i;
	return (1);
}

static int	fill_fields(char **out, const char *s, const char *ifs)
{
	size_t	i;
	size_t	idx;
	int		ret;

	i = 0;
	idx = 0;
	while (s[i])
	{
		ret = extract_next_field(s, ifs, &i, &out[idx]);
		if (ret == 0)
			break ;
		if (ret < 0)
		{
			free_strarray(out, idx);
			return (-1);
		}
		idx++;
	}
	out[idx] = NULL;
	return (0);
}

char	**split_fields(const char *s, const char *ifs)
{
	size_t	fields;
	char	**out;

	if (!s || !*s)
		return (NULL);
	fields = count_fields(s, ifs);
	if (fields == 0)
		return (NULL);
	out = malloc(sizeof(char *) * (fields + 1));
	if (!out)
		return (NULL);
	if (fill_fields(out, s, ifs) < 0)
		return (NULL);
	return (out);
}
