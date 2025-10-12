/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 19:16:21 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/12 19:32:30 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static const char	*tok_display(const t_token *token)
{
	if (token == NULL)
		return ("newline");
	if (token->token_kind == TK_PIPE)
		return ("|");
	if (token->token_kind == TK_REDIR_IN)
		return ("<");
	if (token->token_kind == TK_REDIR_OUT)
		return (">");
	if (token->token_kind == TK_REDIR_APP)
		return (">>");
	if (token->token_kind == TK_HEREDOC)
		return ("<<");
	if (token->token_kind == TK_EOF)
		return ("newline");
	return ("newline");
}

static void	syntax_error_tok(const t_token *token)
{
	const char	*disp = tok_display(token);

	write(2, "minishell: syntax error near unexpected token `", 47);
	write(2, disp, ft_strlen(disp));
	write(2, "'\n", 2);
}

/* --- 事前チェック：OKなら1、エラーで0（メッセージ出力済み） --- */
int	precheck_syntax(const t_tokvec *tv)
{
	if (!tv || !tv->vector || tv->len == 0)
		return (0);

	const t_token *vector = tv->vector;
	size_t vector_size = tv->len;

	/* 末尾は TK_EOF が来る前提（レクサ側で必ず付ける） */
	if (vector[vector_size - 1].token_kind != TK_EOF)
	{
		/* 必要ならここで自動補完しても良いが、まずはエラーにする方針で */
		syntax_error_tok(NULL); /* newline */
		return (0);
	}

	/* 先頭 '|' はエラー */
	if (vector[0].token_kind == TK_PIPE)
	{
		syntax_error_tok(&vector[0]);
		return (0);
	}

	size_t i;
	i = 0;
	/* 線形走査でチェック */
	while (i < vector_size)
	{
		t_tok_kind kind = vector[i].token_kind;

		if (kind == TK_EOF)
			break ;

		if (kind == TK_PIPE)
		{
			/* 連続 ||, 末尾 |, 直前が | の3パターンを弾く */
			if (i == 0)
			{
				syntax_error_tok(&vector[i]);
				return (0);
			} // 先頭 |
			if (vector[i + 1].token_kind == TK_PIPE)
			{
				syntax_error_tok(&vector[i + 1]);
				return (0);
			} // ||
			if (vector[i + 1].token_kind == TK_EOF)
			{
				syntax_error_tok(&vector[i + 1]);
				return (0);
			} // | <EOF>
		}
		else if (is_redir_op(kind))
		{
			/* リダイレクト直後は必ず WORD */
			if (vector[i + 1].token_kind != TK_WORD)
			{
				/* 直後が EOF なら newline を指す */
				const t_token *bad = (vector[i
						+ 1].token_kind == TK_EOF) ? &vector[i + 1] : &vector[i
					+ 1];
				syntax_error_tok(bad);
				return (0);
			}
			/* OK: i と i+1 が (OP, WORD) のペアなので、ループ自体はそのまま */
		}
		/* TK_WORD は自由。その他の未知種別は弾く（防御） */
		i++;
	}

	return (1);
}