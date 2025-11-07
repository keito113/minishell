#ifndef MINISHELL_H
# define MINISHELL_H

# include "builtin.h"
# include "env.h"
# include "error.h"
# include "exec.h"
# include "expand.h"
# include "heredoc.h"
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

#endif
