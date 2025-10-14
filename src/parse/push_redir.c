/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 19:17:39 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/14 13:46:35 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_rtype	kind_from_token(t_tok_kind kind)
{
	if (kind == TK_REDIR_IN)
		return (R_IN);
	if (kind == TK_REDIR_OUT)
		return (R_OUT);
	if (kind == TK_REDIR_APP)
		return (R_APP);
	if (kind == TK_HEREDOC)
		return (R_HDOC);
	return (-1);
}

static char	*dup_token_arg(const t_token *token)
{
	if (token->args)
		return (ft_strdup(token->args));
	return (ft_strdup(""));
}
static void	append_redir(t_redir **head, t_redir *redir)
{
	t_redir	**tail;

	tail = head;
	while (*tail)
		tail = &(*tail)->next;
	*tail = redir;
}

static void	init_redir_fields(t_redir *redir, t_rtype kind, const t_token *tok)
{
	redir->kind = kind;
	if (kind == R_HDOC)
		redir->quoted = tok->hdoc_quoted;
	else
		redir->quoted = 0;
	if (tok->fd_left >= 0)
		redir->fd_target = tok->fd_left;
	else
		redir->fd_target = -1;
	redir->hdoc_fd = -1;
	redir->next = NULL;
}

int	push_redir(t_cmd *cmd, const t_token *redir_tok, const t_token *word_tok)
{
	t_redir	*new_redir;
	t_rtype	kind;

	if (!cmd || !redir_tok || !word_tok || word_tok->token_kind != TK_WORD)
		return (-1);
	kind = kind_from_token(redir_tok->token_kind);
	if (kind < 0)
		return (-1);
	new_redir = malloc(sizeof(*new_redir));
	if (!new_redir)
		return (-1);
	new_redir->arg = dup_token_arg(word_tok);
	if (!new_redir->arg)
	{
		free(new_redir);
		return (-1);
	}
	init_redir_fields(new_redir, kind, redir_tok);
	append_redir(&cmd->redirs, new_redir);
	return (0);
}
