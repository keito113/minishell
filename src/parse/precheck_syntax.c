/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   precheck_syntax.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 18:54:34 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/14 17:56:13 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	syntax_fail_after_redir(const t_token *vector, size_t i,
		t_shell *sh)
{
	const t_token	*bad;

	if (vector[i + 1].token_kind == TK_EOF)
		bad = NULL;
	else
		bad = &vector[i + 1];
	return (syntax_fail(bad, sh));
}

static int	check_token(const t_token *vector, size_t size, size_t i,
		t_shell *sh)
{
	const t_tok_kind	kind = vector[i].token_kind;

	if (kind == TK_EOF)
		return (0);
	if (kind == TK_PIPE)
	{
		if (i == 0)
			return (syntax_fail(&vector[i], sh));
		if (i + 1 >= size || vector[i + 1].token_kind == TK_PIPE)
			return (syntax_fail(&vector[i + 1], sh));
		if (vector[i + 1].token_kind == TK_EOF)
			return (syntax_fail(&vector[i], sh));
	}
	else if (is_redir_op(kind))
	{
		if (i + 1 >= size || vector[i + 1].token_kind != TK_WORD)
			return (syntax_fail_after_redir(vector, i, sh));
	}
	return (1);
}

int	precheck_syntax(const t_tokvec *tv, t_shell *sh)
{
	const t_token	*vector;
	size_t			vector_size;
	size_t			i;

	if (!tv || !tv->vector || tv->len == 0)
		return (0);
	vector = tv->vector;
	vector_size = tv->len;
	if (vector[vector_size - 1].token_kind != TK_EOF)
		return (syntax_fail(NULL, sh));
	if (vector[0].token_kind == TK_PIPE)
		return (syntax_fail(&vector[0], sh));
	i = 0;
	while (i < vector_size)
	{
		if (vector[i].token_kind == TK_EOF)
			break ;
		if (!check_token(vector, vector_size, i, sh))
			return (0);
		i++;
	}
	return (1);
}
