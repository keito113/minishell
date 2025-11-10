/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 16:55:59 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/10 09:02:07 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "builtin.h"
# include "env.h"
# include "error.h"
# include "exec.h"
# include "expand.h"
# include "heredoc.h"
# include "input.h"
# include "libft.h"
# include "parse.h"
# include "pipe.h"
# include "signals.h"
# include "struct.h"
# include "tokens.h"
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/select.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

int	init_shell(t_shell *sh, char **envp);
int	handle_line(char *line, t_shell *sh);
int	run_prompt_loop(t_shell *sh);

#endif
