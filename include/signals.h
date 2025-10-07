/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 13:48:27 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/07 15:37:40 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_H
# define SIGNALS_H

# define _POSIX_C_SOURCE 200809L

# include <signal.h>
# include <stdlib.h>
# include <unistd.h>

extern volatile sig_atomic_t	g_sig;

#endif
