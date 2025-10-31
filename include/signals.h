/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 13:48:27 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/31 11:10:47 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_H
# define SIGNALS_H

# define _POSIX_C_SOURCE 200809L

# include <signal.h>
# include <stdlib.h>
# include <unistd.h>

extern volatile sig_atomic_t	g_sig;

void							sig_setup_readline(void);
void							sig_setup_parent_wait(void);
void							sig_setup_child_exec(void);

#endif
