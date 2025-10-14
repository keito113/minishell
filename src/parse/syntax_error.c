/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 19:16:21 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/13 18:55:19 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static const char	*tok_display(const t_token *token)
{
	if (token == NULL)
		return ("newline");
	if (token->token_kind == TK_PIPE)
		return ("|");
	if (token->token_kind == TK_REDIR_IN)
		return ("<");
	if (token->token_kind == TK_REDIR_OUT)
		return (">");
	if (token->token_kind == TK_REDIR_APP)
		return (">>");
	if (token->token_kind == TK_HEREDOC)
		return ("<<");
	if (token->token_kind == TK_EOF)
		return ("newline");
	return ("newline");
}

static void	syntax_error_tok(const t_token *token)
{
	const char	*disp = tok_display(token);

	write(2, "minishell: syntax error near unexpected token `", 47);
	write(2, disp, ft_strlen(disp));
	write(2, "'\n", 2);
}

int	syntax_fail(const t_token *tok)
{
	syntax_error_tok(tok);
	return (0);
}
