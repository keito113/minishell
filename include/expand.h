/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:22:57 by takawagu          #+#    #+#             */
/*   Updated: 2025/11/04 13:23:20 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPAND_H
# define EXPAND_H

# include "struct.h"

int			expand(t_ast *node, t_shell *sh);
char		*expand_word(const t_wordinfo *info, t_shell *sh);
char		*expand_chunk(const char *chunk, t_shell *sh);
char		*append_literal(const char *text);
const char	*lookup_env(const char *name, t_shell *sh);
char		*join_and_free(char *left, const char *right);
char		*join_num_and_free(char *left, int n);
int			is_valid_var_head(char c);
size_t		var_length(const char *str);

#endif
