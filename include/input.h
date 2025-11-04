/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 12:14:15 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/03 12:41:38 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INPUT_H
# define INPUT_H

# include "libft.h"
# include "signals.h"
# include <readline/history.h>
# include <readline/readline.h>
# include <stdlib.h>

void	ms_input_init(void);
int		has_non_space_tab(const char *str);
char	*ms_readline(const char *ps1);

#endif
