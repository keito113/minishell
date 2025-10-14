/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 18:51:03 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/14 13:47:44 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_redir_op(t_tok_kind kind)
{
	return (kind == TK_REDIR_IN || kind == TK_REDIR_OUT || kind == TK_REDIR_APP
		|| kind == TK_HEREDOC);
}

static int	handle_word(t_cmd *cmd, const t_token *token, size_t *idx)
{
	if (push_word_to_argv(cmd, &token[*idx]) != 0)
		return (-1);
	(*idx)++;
	return (0);
}

static int	handle_redir(t_cmd *cmd, const t_token *token, size_t len,
		size_t *idx)
{
	if (*idx + 1 >= len || token[*idx + 1].token_kind != TK_WORD)
		return (-1);
	if (push_redir(cmd, &token[*idx], &token[*idx + 1]) != 0)
		return (-1);
	*idx += 2;
	return (0);
}

static int	dispatch_token(t_cmd *cmd, const t_token *token, size_t len,
		size_t *idx)
{
	const t_tok_kind	kind = token[*idx].token_kind;

	if (kind == TK_WORD)
		return (handle_word(cmd, token, idx));
	if (is_redir_op(kind))
		return (handle_redir(cmd, token, len, idx));
	return (-1);
}

t_ast	*parse_command(const t_token *token, size_t len, size_t *idx)
{
	t_cmd				cmd;
	t_ast				*node;
	const t_tok_kind	kind = token[*idx].token_kind;

	cmd = (t_cmd){0};
	while (*idx < len)
	{
		if (kind == TK_PIPE || kind == TK_EOF)
			break ;
		if (dispatch_token(&cmd, token, len, idx) != 0)
			return (parse_command_fail(&cmd));
	}
	if (!cmd.argv && !cmd.redirs)
		return (parse_command_fail(&cmd));
	node = malloc(sizeof(*node));
	if (!node)
		return (parse_command_fail(&cmd));
	node->type = AST_CMD;
	node->as.cmd = cmd;
	return (node);
}
