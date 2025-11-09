/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:14:58 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/09 20:42:32 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_H
# define PARSE_H

# include "struct.h"

t_ast		*parse(t_tokvec *tokenvec, t_shell *sh);
t_ast		*parse_command(const t_token *vector, size_t len, size_t *idx);
int			is_redir_op(t_tok_kind k);
int			precheck_syntax(const t_tokvec *tv, t_shell *sh);
int			push_word_to_argv(t_cmd *cmd, const t_token *token);
int			push_redir(t_cmd *cmd, const t_token *op_tok,
				const t_token *word_tok);
void		free_cmd(t_cmd *cmd);
void		free_argv(char **argv);
t_ast		*parse_command_fail(t_cmd *cmd);
int			syntax_fail(const t_token *tok, t_shell *sh);
void		free_ast(t_ast *node);
t_ast		*make_ast(t_tokvec *tokenvec, t_shell *shell);
int			append_wordinfo(t_cmd *cmd, const t_wordinfo *info);
t_wordinfo	*clone_wordinfo(const t_wordinfo *src);
void		free_wordinfo(t_wordinfo *info);
void		free_cmd_wordinfos(t_cmd *cmd);
t_wordinfo	*dup_wordinfo(const t_wordinfo *src);
int			push_word_info(t_cmd *cmd, const t_token *token);
void		free_wordinfo_dup(t_wordinfo *info);
int			is_invalid_fd(int fd);
int			report_bad_fd(int fd);

#endif
