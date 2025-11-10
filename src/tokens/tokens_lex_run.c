/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_lex_run.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 16:38:08 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/10 16:57:42 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	lex_open_quote(t_lexctx *cx, t_quote_kind qk)
{
	int	rc;

	rc = lex_enter_quote(qk, &cx->wb, &cx->st);
	if (rc != TOK_OK)
		return (rc);
	cx->i++;
	return (TOK_OK);
}

static int	lex_step_normal(t_lexctx *cx)
{
	int	rc;

	if (is_space_tab(cx->s[cx->i]))
	{
		rc = lex_handle_space_or_end(&cx->wb, cx->out);
		if (rc != TOK_OK)
			return (rc);
		cx->i++;
		return (TOK_OK);
	}
	else if (cx->s[cx->i] == '|' || cx->s[cx->i] == '<' || cx->s[cx->i] == '>')
		return (lex_handle_operator(cx->s, &cx->i, &cx->wb, cx->out));
	else if (cx->s[cx->i] == '\'')
		return (lex_open_quote(cx, SINGLE));
	else if (cx->s[cx->i] == '"')
		return (lex_open_quote(cx, DOUBLE));
	rc = lex_handle_char(cx->s[cx->i], cx->st, &cx->wb);
	if (rc != TOK_OK)
		return (rc);
	cx->i++;
	return (TOK_OK);
}

static int	lex_step_quote(t_lexctx *cx, char close_ch, t_quote_kind qk)
{
	int	rc;

	if (cx->s[cx->i] == close_ch)
	{
		rc = wb_end_part(&cx->wb, qk);
		if (rc != TOK_OK)
			return (rc);
		cx->st = LXS_NORMAL;
		cx->i++;
		return (TOK_OK);
	}
	else
	{
		rc = lex_handle_char(cx->s[cx->i], cx->st, &cx->wb);
		if (rc != TOK_OK)
			return (rc);
		cx->i++;
		return (TOK_OK);
	}
}

int	lex_run(t_lexctx *cx)
{
	int	rc;

	rc = TOK_OK;
	while (cx->s[cx->i] != '\0')
	{
		if (cx->st == LXS_NORMAL)
			rc = lex_step_normal(cx);
		else if (cx->st == LXS_IN_SQ)
			rc = lex_step_quote(cx, '\'', SINGLE);
		else
			rc = lex_step_quote(cx, '"', DOUBLE);
		if (rc != TOK_OK)
			return (rc);
	}
	return (TOK_OK);
}
