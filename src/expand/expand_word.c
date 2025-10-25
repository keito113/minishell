/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_word.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 17:22:25 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/20 15:44:19 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*join_and_free(char *left, const char *right)
{
	char	*joined;

	if (!left && !right)
		return (NULL);
	if (!left)
		return (ft_strdup(right));
	if (!right)
		return (left);
	joined = ft_strjoin(left, right);
	free(left);
	return (joined);
}

static char	*join_plain_piece(char *acc, const char *text)
{
	char	*piece;
	char	*next;

	piece = append_literal(text);
	if (!piece)
		return (free(acc), NULL);
	next = join_and_free(acc, piece);
	free(piece);
	return (next);
}

static char	*join_expanded_piece(char *acc, const char *text, t_shell *sh)
{
	char	*piece;
	char	*next;

	piece = expand_chunk(text, sh);
	if (!piece)
		return (free(acc), NULL);
	next = join_and_free(acc, piece);
	free(piece);
	return (next);
}

static char	*join_parts_verbatim(const t_wordinfo *info)
{
	char	*result;
	size_t	i;
	char	*next;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	while (i < info->parts_count)
	{
		next = join_plain_piece(result, info->parts[i].text);
		if (!next)
			return (NULL);
		result = next;
		i++;
	}
	return (result);
}

char	*expand_word(const t_wordinfo *info, t_shell *sh)
{
	char			*result;
	size_t			i;
	const t_parts	*part;

	i = 0;
	part = &info->parts[i];
	if (!info)
		return (NULL);
	if (!info->had_dollar)
		return (join_parts_verbatim(info));
	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (i < info->parts_count)
	{
		if (part->quote == SINGLE)
			result = join_plain_piece(result, part->text);
		else
			result = join_expanded_piece(result, part->text, sh);
		if (!result)
			return (NULL);
		i++;
	}
	return (result);
}
