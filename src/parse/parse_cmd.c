/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 18:51:03 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/12 19:54:08 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_redir_op(t_tok_kind k)
{
	return (k == TK_REDIR_IN || k == TK_REDIR_OUT || k == TK_REDIR_APP
		|| k == TK_HEREDOC);
}
static char	*x_strdup(const char *str)
{
	size_t	n;
	char	*ptr;

	if (!str)
		return (NULL);
	n = ft_strlen(str);
	ptr = (char *)malloc(n + 1);
	if (!ptr)
		return (NULL);
	ft_memcpy(ptr, str, n + 1);
	return (ptr);
}

static int	push_word_to_argv(t_cmd *cmd, const t_token *tok)
{
	char	**oldv;
	char	**nv;
	size_t	n;
	char	*word;

	if (!cmd || !tok || tok->token_kind != TK_WORD)
		return (-1);
	/* 追加する文字列（ここではトークンの生文字列をそのまま複製）
		後工程で展開/quote除去するなら、ここはそのままでOK */
	word = tok->args ? x_strdup(tok->args) : x_strdup("");
	if (!word)
		return (-1);
	oldv = cmd->argv;
	n = 0;
	if (oldv)
		while (oldv[n])
			++n;
	/* 新配列: 既存 n 要素 + 新要素 1 + 終端 NULL */
	nv = (char **)malloc(sizeof(char *) * (n + 2));
	if (!nv)
	{
		free(word);
		return (-1);
	}
	/* 旧要素ポインタをそのままコピー（中身の所有権は旧→新へ移すイメージ） */
	for (size_t i = 0; i < n; ++i)
		nv[i] = oldv[i];
	nv[n] = word;     /* 追加要素 */
	nv[n + 1] = NULL; /* 終端 */
	/* 旧の「配列本体」だけ解放（各文字列は nv に移管済み） */
	free(oldv);
	cmd->argv = nv;
	return (0);
}

/* --- コマンド1個を tokens[idx..] から読む（PIPE/EOF で終わる）--- */

t_ast	*parse_command(const t_token *vector, size_t len, size_t *idx)
{
	t_cmd cmd;
	t_tok_kind kind;
	t_ast *node;

	cmd = (t_cmd){0};
	while (*idx < len)
	{
		kind = vector[*idx].token_kind;
		if (kind == TK_PIPE || kind == TK_EOF)
			break ;
		if (kind == TK_WORD)
		{
			push_word_to_argv(&cmd, &vector[*idx]);
			(*idx)++;
		}
		else if (is_redir_op(kind))
		{
			if (*idx + 1 >= len || vector[*idx + 1].token_kind != TK_WORD)
			{
				// syntax error: リダイレクトの直後に WORD が必要
				free_cmd(&cmd);
				return (NULL);
			}
			push_redir(&cmd, &vector[*idx], &vector[*idx + 1]);
			*idx += 2;
		}
		else
		{
			// 予期しないトークン
			free_cmd(&cmd);
			return (NULL);
		}
	}
	/* 空コマンド対策：argv も redirs も無いのはエラーにするならここで弾く */
	// if (!cmd.argv && !cmd.redirs) { ... }
	node = malloc(sizeof(*node));
	if (!node)
	{
		free_cmd(&cmd);
		return (NULL);
	}
	node->type = AST_CMD;
	node->as.cmd = cmd; /* 所有権をムーブ */
	return (node);
}