/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 08:51:27 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/03 10:19:45 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ECHO_H
# define ECHO_H

# include "libft.h"
# include "minishell.h"
# include <unistd.h>

int	builtin_echo(char **av, int out_fd);

#endif
