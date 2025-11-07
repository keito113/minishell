/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 11:44:56 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/07 18:59:27 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENS_H
# define TOKENS_H

# include "libft.h"
# include "struct.h"
# include <stddef.h>
# include <stdlib.h>
# include <sys/types.h>

// tokens_utils.c
void	wb_init(t_wbuild *wb);
int		is_space_tab(char c);
void	read_op(const char *s, size_t i, t_tok_kind *k, size_t *len);
char	*strndup_x(const char *p, size_t n);
size_t	parts_total_len(const t_wordinfo *w);

// tokens_utils_fd.c
int		parse_fd_int_slice(const char *s, size_t l, size_t r, int *out_fd);
int		read_fd_left(const char *s, size_t op_pos, int *out_fd);

// tokens_word.c
int		wb_begin_part(t_wbuild *wb, t_quote_kind q);
int		wb_putc(t_wbuild *wb, char c, int in_squote, int in_dquote);
int		wb_end_part(t_wbuild *wb, t_quote_kind q);
int		wb_emit_word_token(t_wbuild *wb, t_tokvec *out);

// tokens_vec_free.c
void	tokvec_free(t_tokvec *v);

// tokens_vec.c
void	tokvec_init(t_tokvec *v);
int		tokvec_push_word(t_tokvec *v, t_wordinfo w);
int		tokvec_push_op(t_tokvec *v, t_tok_kind kind, int fd_left,
			int hdoc_quoted);
int		tokvec_push_eof(t_tokvec *v);

// tokens_lex_helpers.c
int		lex_handle_space_or_end(t_wbuild *wb, t_tokvec *out);
int		lex_handle_operator(const char *s, size_t *i, t_wbuild *wb,
			t_tokvec *out);
int		lex_enter_quote(t_quote_kind qkind, t_wbuild *wb, int *st);
int		lex_handle_char(char c, int st, t_wbuild *wb);
void	wb_cleanup(t_wbuild *wb);

// tokens_lex.c
int		tok_lex_line(const char *s, t_tokvec *out, int *err);

// tokens_post.c
int		tok_is_redir(t_tok_kind k);
void	finalize_hdoc_flags(t_tokvec *tv);
int		syntax_check(const t_tokvec *tv);
void	finalize_word_args(t_tokvec *tv);

int		lexer(char *line, t_tokvec *tv, t_shell *sh);

#endif
