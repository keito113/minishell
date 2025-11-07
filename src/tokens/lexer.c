/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 18:46:32 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/07 18:47:03 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	lexer(char *line, t_tokvec *tv, t_shell *sh)
{
	int	lex_err;
	int	rc;

	lex_err = 0;
	rc = tok_lex_line(line, tv, &lex_err);
	finalize_hdoc_flags(tv);
	finalize_word_args(tv);
	if (rc != TOK_OK)
	{
		sh->last_status = 2;
		return (-1);
	}
	return (0);
}
