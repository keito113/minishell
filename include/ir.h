/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ir.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 11:44:46 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/02 11:57:24 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IR_H
# define IR_H
# include "tokens.h"

typedef enum e_rkind
{
	R_IN,
	R_OUT,
	R_APP,
	R_HERE
}						t_rkind;

typedef struct s_redir_ir
{
	t_rkind				kind;
	char *word;      /* ファイル名 or ヒアドキュメントのリミッタ */
	int here_quoted; /* リミッタが引用されていたか */
	int hdoc_fd;     /* 準備済みFD（未準備は -1） */
	struct s_redir_ir	*next;
}						t_redir_ir;

typedef struct s_cmd_ir
{
	char **argv;        /* まだ展開/クォート除去 前 */
	t_redir_ir *redirs; /* 左→右 */
}						t_cmd_ir;

typedef struct s_line_ir
{
	t_list *cmds; /* t_cmd_ir* のリスト（パイプ順） */
}						t_line_ir;

int						ir_from_tokens(t_list *tok, t_line_ir **out,
							int *errcode);
void					ir_free(t_line_ir **p);

#endif
