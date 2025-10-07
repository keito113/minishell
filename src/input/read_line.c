/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_line.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitabe <keitabe@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 12:07:15 by keitabe           #+#    #+#             */
/*   Updated: 2025/10/07 12:46:22 by keitabe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/input.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>

void	ms_input_init(void)
{
	rl_catch_signals = 0;
}

int	has_non_space_tab(const char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] != ' ' && str[i] != '\t')
			return (1);
		i++;
	}
	return (0);
}

char	*ms_readline(const char *ps1)
{
	const char	*prompt;
	char		*line;

	if (!ps1)
		prompt = "minishell$ ";
	else
		prompt = ps1;
	line = readline(prompt);
	if (!line)
		return (NULL);
	if (g_sig)
	{
		free(line);
		g_sig = 0;
		return (ft_strdup(""));
	}
	if (has_non_space_tab(line))
		add_history(line);
	return (line);
}
