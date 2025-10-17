/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_vec_free.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 07:49:30 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/15 08:30:21 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

static void	free_one_token(t_token *t)
{
	size_t	i;

	if (t->args)
		free(t->args);
	if (t->token_kind == TK_WORD && t->word_info.parts)
	{
		i = 0;
		while (i < t->word_info.parts_count)
			free(t->word_info.parts[i++].text);
		free(t->word_info.parts);
		t->word_info.parts = NULL;
		t->word_info.parts_count = 0;
		t->word_info.had_dollar = 0;
		t->word_info.had_quotes = 0;
	}
	t->args = NULL;
	t->fd_left = -1;
	t->hdoc_quoted = 0;
}

void	tokvec_free(t_tokvec *v)
{
	size_t	i;

	i = 0;
	while (i < v->len)
	{
		free_one_token(&v->vector[i]);
		i++;
	}
	free(v->vector);
	v->vector = NULL;
	v->len = 0;
	v->cap = 0;
}
