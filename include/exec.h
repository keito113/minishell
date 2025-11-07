/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:23:32 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/07 18:43:26 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

# include "struct.h"

int		execute_ast(t_ast *ast, t_shell *sh);
int		exec_entry(t_ast *root, t_shell *sh);
int		run_single_command(t_cmd *cmd, t_shell *sh);
int		apply_redirs(const t_cmd *cmd);
void	exec_external(char *const argv[], t_shell *sh);
char	*find_cmd_path(const char *cmd, t_shell *sh);
int		open_src_fd(t_redir *redir);
void	exec_child_single(t_cmd *cmd, t_shell *sh);
void	cleanup_child_and_exit(t_shell *sh, int status);

#endif
