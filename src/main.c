/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 12:28:33 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/21 12:46:14 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/input.h" // ms_input_init, ms_readline
#include "minishell.h"     // t_shell, t_ast, make_ast, free_ast
#include <stdio.h>
#include <stdlib.h>

// 変数1個
static void	dump_redirs(const t_redir *r)
{
	const char	*k;

	while (r)
	{
		k = (r->kind == R_IN) ? "<" : (r->kind == R_OUT) ? ">" : (r->kind == R_APP) ? ">>" : "<<";
		printf("  redir: %s arg=\"%s\" fd=%d quoted=%d\n", k,
			r->arg ? r->arg : "", r->fd_target, r->quoted);
		r = r->next;
	}
}

// 変数2個
static void	dump_ast(const t_ast *n, int depth)
{
	int		i;
	size_t	j;

	if (!n)
		return ;
	for (i = 0; i < depth; i++)
		putchar(' ');
	if (n->type == AST_PIPE)
	{
		puts("[PIPE]");
		dump_ast(n->as.pipe.left, depth + 2);
		dump_ast(n->as.pipe.right, depth + 2);
	}
	else
	{
		puts("[CMD]");
		if (n->as.cmd.argv)
		{
			j = 0;
			while (n->as.cmd.argv[j])
			{
				for (i = 0; i < depth + 2; i++)
					putchar(' ');
				printf("argv[%zu]=\"%s\"\n", j, n->as.cmd.argv[j]);
				j++;
			}
		}
		dump_redirs(n->as.cmd.redirs);
	}
}

// 変数3個
static t_ast	*lex_post_parse(const char *line, t_shell *sh)
{
	t_tokvec	tv;
	int			err;
	t_ast		*root;

	tokvec_init(&tv);
	err = 0;
	if (tok_lex_line(line, &tv, &err) < 0)
	{
		fprintf(stderr, "[LEX_ERR] code=%d\n", err);
		tokvec_free(&tv);
		return (NULL);
	}
	// tokens の事後処理（HEREDOC の quoted/limiter と、全 WORD の args 生成）
	finalize_hdoc_flags(&tv);
	finalize_word_args(&tv);
	// 構文は parse 側の precheck_syntax で判定される
	root = make_ast(&tv, sh);
	tokvec_free(&tv);
	return (root);
}

// 変数3個
int	main(void)
{
	t_shell	sh;
	char	*line;
	t_ast	*root;

	sh = (t_shell){0};
	ms_input_init();
	while (1)
	{
		line = ms_readline("minishell$ ");
		if (!line)
		{
			puts("exit");
			break ;
		}
		if (!*line)
		{
			free(line);
			continue ;
		} // Ctrl-C 空行
		root = lex_post_parse(line, &sh);
		free(line);
		if (!root)
			continue ;
		dump_ast(root, 0);
		free_ast(root);
	}
	return (0);
}
