/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 17:24:16 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/22 19:33:34 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*join_num_and_free(char *left, int n)
{
	char	*tmp;
	char	*joined;

	tmp = ft_itoa(n);
	if (!tmp)
		return (free(left), NULL);
	joined = join_and_free(left, tmp);
	free(tmp);
	return (joined);
}

int	is_valid_var_head(char c)
{
	return (ft_isalpha((unsigned char)c) || c == '_');
}

size_t	var_length(const char *str)
{
	size_t	len;

	len = 0;
	while (str[len] && (ft_isalnum((unsigned char)str[len]) || str[len] == '_'))
		len++;
	return (len);
}

char	*append_literal(const char *text)
{
	if (!text)
		return (ft_strdup(""));
	return (ft_strdup(text));
}
