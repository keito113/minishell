/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:25:54 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/07 14:25:14 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
# define ERROR_H

# include "struct.h"

int		return_laststatus(t_shell *sh, int error_code);
int		status_to_exitcode(int wstatus);
void	cmd_not_found_exit(const char *cmd, t_shell *sh);
void	permission_denied_exit(const char *target, t_shell *sh);

#endif
