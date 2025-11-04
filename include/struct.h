/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:20:51 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/04 13:21:18 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCT_H
# define STRUCT_H

# include "libft.h"
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

# ifndef MINISHELL_FD_UPPER
#  define MINISHELL_FD_UPPER 1048576
# endif

typedef struct s_env
{
	char			*key;
	char			*val;
	int				exported;
	struct s_env	*next;
}	t_env;

typedef struct s_shell
{
	char	**envp;
	int		last_status;
	int		interactive;
	t_env	*env;
	int		exit_status;
	int		should_exit;
}	t_shell;

typedef struct s_redir
{
	t_rtype				kind;
	char				*arg;
	t_wordinfo			*word_info;
	int					here_doc_quoted;
	int					fd_target;
	int					hdoc_fd;
	struct s_redir		*next;
}	t_redir;

typedef struct s_cmd
{
	char		**argv;
	t_wordinfo	**word_infos;
	size_t		argc;
	t_redir		*redirs;
	int			is_builtin;
	int			is_child;
	t_shell		*sh;
}	t_cmd;

typedef struct s_ast
{
	t_ast_type	type;
	union
	{
		struct
		{
			struct s_ast	*left;
			struct s_ast	*right;
		}	pipe;
		t_cmd			cmd;
	}	as;
}	t_ast;

typedef struct s_parse_ctx
{
	const t_token	*tokens;
	size_t			len;
	size_t			index;
	t_shell			*sh;
}	t_parse_ctx;

typedef struct s_pipe_ctx
{
	int		prev_read;
	int		pipefd[2];
	int		need_pipe_out;
	pid_t	last_pid;
}	t_pipe_ctx;

#endif
