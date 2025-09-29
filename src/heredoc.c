/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 09:02:27 by takawagu          #+#    #+#             */
/*   Updated: 2025/09/29 11:42:06 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	prepare_one_heredoc(t_redir *redir, t_shell *sh, t_ast *node)
{
	int	fd[2];
	int	write;
	int	read;
	int	status;

	if (pipe(fd) == -1)
		return (1);
	write = fd[1];
	read = fd[0];
	status = read_heredoc_into_fd(write, redir, sh);
	close(write);
	if (status != 0)
	{ // 130 や 1
		close(read);
		close_all_prepared_hdocs(node);
		return (status);
	}
	redir->hdoc_fd = read; // 実行時に dup2 するため保存
	return (0);
}
int	prepare_cmd_heredocs(t_cmd *cmd, t_shell *sh, t_ast *node)
{
	t_redir	*redir;

	redir = cmd->redirs;
	while (redir)
	{
		if (redir->kind == R_HDOC)
		{
			sh->last_status = prepare_one_heredoc(redir, sh, node);
			if (sh->last_status != 0)
				return (sh->last_status);
		}
		redir = redir->next;
	}
	return (0);
}

int	prepare_heredocs(t_ast *node, t_shell *sh)
{
	if (!node)
		return (0);
	if (node->type == AST_CMD)
		return (prepare_cmd_heredocs(&node->as.cmd, sh, node));
	// AST_PIPE
	sh->last_status = prepare_heredocs(node->as.pipe.left, sh);
	if (sh->last_status)
		return (sh->last_status);
	return (prepare_heredocs(node->as.pipe.right, sh));
}

int	read_heredoc_into_fd(int write_fd, t_redir *redir, t_shell *sh)
{
	char	*limmiter;
	char	*line;

	(void)sh;
	limmiter = redir->arg;
	while ((line = readline("heredoc> ")) != NULL)
	{
		/* リミッタ完全一致で終了（改行は含まれない） */
		if (ft_strcmp(line, limmiter) == 0)
		{
			free(line);
			break ;
		}
		// /* quoted==0 のときだけ $展開 */
		// if (redir->quoted == 0)
		// {
		// 	char *expanded = expand_dollar(line, sh);
		// 	if (!expanded)
		// 	{
		// 		free(line);
		// 		return (1);
		// 	} // メモリ不足
		// 	free(line);
		// 	line = expanded;
		// }
		if (write(write_fd, line, ft_strlen(line)) < 0)
		{
			free(line);
			return (1);
		}
		if (write(write_fd, "\n", 1) < 0)
		{
			free(line);
			return (1);
		}
		free(line);
	}
	return (0); /* EOFで抜けた場合も今は成功扱い */
}
