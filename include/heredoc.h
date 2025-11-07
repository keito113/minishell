/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:24:19 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/07 12:21:01 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEREDOC_H
# define HEREDOC_H

# include "struct.h"

void	close_hdocs_in_cmd(t_cmd *cmd);
void	close_all_prepared_hdocs(t_ast *node);
int		prepare_cmd_heredocs(t_cmd *cmd, t_shell *sh, t_ast *node);
int		prepare_heredocs(t_ast *node, t_shell *sh);
int		heredoc_handle_line(int wfd, t_redir *r, t_shell *sh, char *line);
int		hdoc_line_matches(const char *line, const char *limiter);
int		heredoc_loop(int wfd, t_redir *redir, t_shell *sh);
int		read_heredoc_into_fd(int write_fd, t_redir *redir, t_shell *sh);

#endif
