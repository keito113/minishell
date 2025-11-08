/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 12:28:33 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/08 15:55:55 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// src/main.c
#include "input.h"
#include "minishell.h" // ← parse / exec_entry / expand / env_*
#include "signals.h"
#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int	init_shell(t_shell *sh, char **envp)
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

static int	handle_line(char *line, t_shell *sh)
{
	t_tokvec	tv;
	t_ast		*ast;
	int			rc;
	int			lex_err;

	ast = NULL;
	lex_err = 0;
	if (!line || line[0] == '\0')
		return (0);
	if (lexer(line, &tv, sh) < 0)
		return (sh->last_status);
	ast = parse(&tv, sh);
	sh->currrent_ast = ast;
	if (!ast)
	{
		sh->last_status = 2;
		return (sh->last_status);
	}
	rc = expand(ast, sh);
	if (rc != 0)
	{
		free_ast(ast);
		sh->last_status = 1;
		return (sh->last_status);
	}
	return (execute_ast(ast, sh));
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	sh;
	char	*line;
	int		st;

	(void)argc;
	(void)argv;
	if (init_shell(&sh, envp) < 0)
	{
		write(STDERR_FILENO, "minishell: failed to init env\n", 30);
		return (1);
	}
	while (1)
	{
		// REPL 用のシグナル設定（Ctrl-C: 新しい行／Ctrl-\ 無視 など）
		sig_setup_readline();
		// input モジュールを使わず素の readline を採用（プロンプトは任意）
		line = ms_readline("minishell$ ");
		// Ctrl-C を受けた直後のフレーム（handler 側で g_sig=1 を想定）
		if (g_sig)
		{
			g_sig = 0;
			sh.last_status = 130; // bash 準拠
			free(line);
			continue ;
		}
		// Ctrl-D（EOF）
		if (!line)
		{
			write(STDOUT_FILENO, "exit\n", 5);
			break ;
		}
		// 履歴（空行は追加しない）
		if (line[0] != '\0')
			add_history(line);
		(void)handle_line(line, &sh);
		free(line);
		if (sh.should_exit)
		{
			st = sh.exit_status & 0xFF;
			/* 必要な後始末（あなたの環境に合わせて） */
			if (sh.env)
				free_env_list(&sh.env);
			rl_clear_history(); /* 任意：readline 履歴の解放 */
			exit(st);
		}
	}
	if (sh.env)
		free_env_list(&sh.env);
	return (sh.last_status);
}
