/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 08:53:33 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/10 10:36:53 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	init_shell(t_shell *sh, char **envp)
{
	ft_memset(sh, 0, sizeof(*sh));
	sh->interactive = isatty(STDIN_FILENO);
	sh->last_status = 0;
	sh->env = NULL;
	sh->envp = envp;
	if (env_init_from_envp(&sh->env, envp) != 0)
	{
		free_env_list(&sh->env);
		return (-1);
	}
	return (0);
}

static int	expand_ast(t_ast *ast, t_shell *sh)
{
	int	r_code;

	r_code = expand(ast, sh);
	if (r_code != 0)
	{
		free_ast(ast);
		sh->current_ast = NULL;
		sh->last_status = 1;
		return (sh->last_status);
	}
	return (0);
}

int	handle_line(char *line, t_shell *sh)
{
	t_tokvec	tv;
	t_ast		*ast;
	int			r_code;

	ast = NULL;
	if (!line || line[0] == '\0')
		return (0);
	if (lexer(line, &tv, sh) < 0)
		return (sh->last_status);
	ast = parse(&tv, sh);
	sh->current_ast = ast;
	if (!ast)
	{
		sh->last_status = 2;
		return (sh->last_status);
	}
	if (expand_ast(ast, sh) != 0)
		return (sh->last_status);
	r_code = execute_ast(ast, sh);
	return (r_code);
}
