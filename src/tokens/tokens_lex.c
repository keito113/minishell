/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_lex.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 11:24:10 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/10 16:55:16 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokens.h"

static void	print_unclosed_quote_error(t_quote_kind kind)
{
	if (kind == SINGLE)
		write(STDERR_FILENO,
			"minishell: syntax error near unexpected token `''\n",
			sizeof("minishell: syntax error near unexpected token `''\n") - 1);
	else
		write(STDERR_FILENO,
			"minishell: syntax error near unexpected token `\"'\n",
			sizeof("minishell: syntax error near unexpected token `\"'\n") - 1);
}

static int	handle_unclosed_quote(const t_lexctx *cx, int rc)
{
	if (rc != TOK_OK)
		return (rc);
	if (cx->st == LXS_IN_SQ)
	{
		print_unclosed_quote_error(SINGLE);
		return (TOK_ERR_UNCLOSED_SQUOTE);
	}
	if (cx->st == LXS_IN_DQ)
	{
		print_unclosed_quote_error(DOUBLE);
		return (TOK_ERR_UNCLOSED_DQUOTE);
	}
	return (rc);
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
	rc = handle_unclosed_quote(&cx, rc);
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
