/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 12:28:33 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/30 12:45:39 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/input.h"
#include "include/minishell.h"
#include <stdio.h>
#include <stdlib.h>

// static const char	*quote_to_str(t_quote_kind q)
//{
//	if (q == SINGLE)
//		return ("SINGLE");
//	if (q == DOUBLE)
//		return ("DOUBLE");
//	return ("NONE");
// }

// static void	dump_wordinfo(const t_wordinfo *info, int depth)
//{
//	size_t	k;
//	int		i;

//	if (!info)
//	{
//		for (i = 0; i < depth; i++)
//			putchar(' ');
//		puts("(no word info)");
//		return ;
//	}
//	for (i = 0; i < depth; i++)
//		putchar(' ');
//	printf("had_quotes=%d had_dollar=%d parts=%zu\n", info->had_quotes,
//		info->had_dollar, info->parts_count);
//	k = 0;
//	while (k < info->parts_count)
//	{
//		for (i = 0; i < depth; i++)
//			putchar(' ');
//		printf("  part[%zu] quote=%s text=\"%s\"\n", k,
//			quote_to_str(info->parts[k].quote),
//			info->parts[k].text ? info->parts[k].text : "");
//		k++;
//	}
//}

// static void	dump_redirs(const t_redir *r)
//{
//	const char	*k;

//	while (r)
//	{
//		k = (r->kind == R_IN) ? "<" : (r->kind == R_OUT) ? ">" : (r->kind == R_APP) ? ">>" : "<<";
//		printf("  redir: %s arg=\"%s\" fd=%d quoted=%d\n", k,
//			r->arg ? r->arg : "", r->fd_target, r->here_doc_quoted);
//		if (r->word_info)
//			dump_wordinfo(r->word_info, 4);
//		r = r->next;
//	}
//}

// static void	dump_ast(const t_ast *n, int depth)
//{
//	int		i;
//	size_t	j;

//	if (!n)
//		return ;
//	for (i = 0; i < depth; i++)
//		putchar(' ');
//	if (n->type == AST_PIPE)
//	{
//		puts("[PIPE]");
//		dump_ast(n->as.pipe.left, depth + 2);
//		dump_ast(n->as.pipe.right, depth + 2);
//	}
//	else
//	{
//		puts("[CMD]");
//		if (n->as.cmd.argv)
//		{
//			j = 0;
//			while (n->as.cmd.argv[j])
//			{
//				for (i = 0; i < depth + 2; i++)
//					putchar(' ');
//				printf("argv[%zu]=\"%s\"\n", j, n->as.cmd.argv[j]);
//				if (n->as.cmd.word_infos)
//					dump_wordinfo(n->as.cmd.word_infos[j], depth + 4);
//				j++;
//			}
//		}
//		dump_redirs(n->as.cmd.redirs);
//	}
//}

static int	tokenize_line(const char *line, t_tokvec *out, int *err)
{
	int	local_err;

	if (!line || !out)
		return (-1);
	tokvec_init(out);
	local_err = 0;
	if (tok_lex_line(line, out, &local_err) < 0)
	{
		fprintf(stderr, "[LEX_ERR] code=%d\n", local_err);
		tokvec_free(out);
		if (err)
			*err = local_err;
		return (-1);
	}
	finalize_hdoc_flags(out);
	finalize_word_args(out);
	if (err)
		*err = local_err;
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell		sh;
	char		*line;
	t_ast		*root;
	t_tokvec	token_vec;

	(void)argc;
	(void)argv;
	sh = (t_shell){0};
	if (env_init_from_envp(&sh.env, envp) < 0)
	{
		free_env_list(&sh.env);
		return (1);
	}
	sh.envp = envp;
	sh.last_status = 0;
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
		}
		if (tokenize_line(line, &token_vec, NULL) != 0)
		{
			free(line);
			continue ;
		}
		root = parse(&token_vec, &sh);
		free(line);
		if (!root)
			continue ;
		if (expand(root, &sh) < 0)
		{
			free_ast(root);
			continue ;
		}
		exec_entry(root, &sh);
		// dump_ast(root, 0);
		free_ast(root);
	}
	free_env_list(&sh.env);
	return (0);
}
