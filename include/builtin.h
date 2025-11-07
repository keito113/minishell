/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:27:07 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/05 15:26:46 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTIN_H
# define BUILTIN_H

# include "struct.h"

int		exec_builtin(t_cmd *cmd, t_env **penv);
int		is_builtin_name(const char *name);
int		builtin_export(char **argv, t_env **penv);
int		handle_export_arg(char *arg, t_env **penv);
void	print_export_sorted(const t_env *env);
int		builtin_env(char **argv, t_env **env);
int		builtin_pwd(char **argv);
int		builtin_unset(char **argv, t_env **penv);
int		builtin_cd(char **argv, t_env **env);
int		builtin_echo(char **argv, int out_fd);
int		builtin_exit(char **argv, t_shell *sh, int is_child);
int		str_to_ll_checked(const char *s, long long *out);
int		is_valid_identifier(const char *s);
void	restore_builtin_redirects(t_fd_backup *backups, size_t len);
int		setup_builtin_redirects(t_cmd *cmd, t_fd_backup **backups, size_t *len);
int		target_fd_from_redir(const t_redir *redir);
int		backup_target_fd(int target_fd, t_fd_backup *arr, size_t *len);

#endif
