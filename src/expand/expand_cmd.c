/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 16:47:46 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/09 18:08:36 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	expand_redir(t_redir *redir, t_shell *sh)
{
	char	*expanded;

	while (redir)
	{
		if (redir->word_info && redir->word_info->had_dollar
			&& !redir->here_doc_quoted)
		{
			expanded = expand_word(redir->word_info, sh);
			if (!expanded)
				return (-1);
			free(redir->arg);
			redir->arg = expanded;
		}
		redir = redir->next;
	}
	return (0);
}

static int	handle_unquoted_expansion(t_expand_ctx *ctx, size_t *index,
		char *expanded)
{
	char	**fields;

	fields = split_fields(expanded, ctx->ifs);
	free(expanded);
	if (!fields)
		return (remove_arg(ctx->cmd, *index));
	if (insert_fields(ctx->cmd, *index, fields) < 0)
	{
		free_strarray(fields, -1);
		return (-1);
	}
	free_strarray(fields, -1);
	(*index)++;
	return (0);
}

static int	process_arg(t_expand_ctx *ctx, size_t *index, t_wordinfo *info)
{
	char	*expanded;

	if (!info || !info->had_dollar)
	{
		(*index)++;
		return (0);
	}
	expanded = expand_word(info, ctx->sh);
	if (!expanded)
		return (-1);
	if (!info->had_quotes)
		return (handle_unquoted_expansion(ctx, index, expanded));
	free(ctx->cmd->argv[*index]);
	ctx->cmd->argv[*index] = expanded;
	(*index)++;
	return (0);
}

int	expand_cmd(t_cmd *cmd, t_shell *sh)
{
	t_expand_ctx	ctx;
	size_t			index;

	if (!cmd)
		return (0);
	ctx.cmd = cmd;
	ctx.sh = sh;
	ctx.ifs = get_ifs_value(sh);
	index = 0;
	while (index < cmd->argc)
	{
		if (process_arg(&ctx, &index, cmd->word_infos[index]) < 0)
			return (-1);
	}
	return (expand_redir(cmd->redirs, sh));
}
