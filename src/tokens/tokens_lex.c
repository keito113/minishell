/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_lex.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 11:24:10 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/04 07:57:07 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

static int	lex_step_normal(t_lexctx *cx)
{
	int	rc;

	if (is_space_tab(cx->s[cx->i]))
		rc = lex_handle_space_or_end(&cx->wb, cx->out);
	else if (cx->s[cx->i] == '|' || cx->s[cx->i] == '<' || cx->s[cx->i] == '>')
		rc = lex_handle_operator(cx->s, &cx->i, &cx->wb, cx->out);
	else if (cx->s[cx->i] == '\'')
		rc = lex_enter_quote(SINGLE, &cx->wb, &cx->st);
	else if (cx->s[cx->i] == '"')
		rc = lex_enter_quote(DOUBLE, &cx->wb, &cx->st);
	else
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

static int	lex_run(t_lexctx *cx)
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

int	tok_lex_line(const char *s, t_tokvec *out, int *err)
{
	t_lexctx	cx;
	int			rc;

	cx.s = s;
	cx.i = 0;
	cx.st = LXS_NORMAL;
	cx.out = out;
	tokvec_init(out);
	wb_init(&cx.wb);
	rc = lex_run(&cx);
	if (rc == TOK_OK && cx.st == LXS_IN_SQ)
		rc = TOK_ERR_UNCLOSED_SQUOTE;
	if (rc == TOK_OK && cx.st == LXS_IN_DQ)
		rc = TOK_ERR_UNCLOSED_DQUOTE;
	if (rc == TOK_OK)
		rc = lex_handle_space_or_end(&cx.wb, cx.out);
	if (rc == TOK_OK)
		rc = tokvec_push_eof(cx.out);
	if (err != NULL)
		*err = rc;
	wb_cleanup(&cx.wb);
	if (rc != TOK_OK)
		return (-1);
	return (0);
}
