/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 17:46:25 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/12 19:33:36 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* --- パイプ列を左結合で組み上げる --- */
t_ast	*make_ast(t_tokvec *tv)
{
	size_t	index;
	t_ast	*left;
	t_ast	*right;
	t_ast	*pipe;

	if (!tv || !tv->vector || tv->len == 0)
		return (NULL);
	if (!precheck_syntax(tv))
		return (NULL);
	index = 0;
	if (tv->vector[index].token_kind == TK_PIPE)
		return (NULL); // 先頭が '|' は構文エラー
	left = parse_command(tv->vector, tv->len, &index);
	if (!left)
		return (NULL);
	/* '|' が出るたびに右の command を読み、PIPE ノードで左結合 */
	while (index < tv->len && tv->vector[index].token_kind == TK_PIPE)
	{
		/* 連続 '||' の検出 */
		if (index + 1 < tv->len && tv->vector[index + 1].token_kind == TK_PIPE)
		{
			// 構文エラー：連続パイプ
			free_ast(left);
			return (NULL);
		}
		index++; /* '|' を消費 */
		right = parse_command(tv->vector, tv->len, &index);
		if (!right)
		{
			// 構文エラー（'|' の後にコマンドが無い等）
			free_ast(left);
			return (NULL);
		}
		pipe = malloc(sizeof(*pipe));
		if (!pipe)
		{
			free_ast(left);
			free_ast(right);
			return (NULL);
		}
		pipe->type = AST_PIPE;
		pipe->as.pipe.left = left;
		pipe->as.pipe.right = right;
		left = pipe; /* 左結合：新しい根に置き換える */
	}
	/* 末尾が '|' で終わっていないかチェック */
	if (index < tv->len && tv->vector[index].token_kind != TK_EOF)
	{
		// 余剰トークン（想定外）→ エラー扱いにするならここで
		free_ast(left);
		return (NULL);
	}
	return (left); /* AST の root */
}
