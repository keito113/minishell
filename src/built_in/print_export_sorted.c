/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_export_sorted.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 16:39:14 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/26 14:31:03 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_exported(const t_env *env)
{
	int	n_exported;

	n_exported = 0;
	while (env)
	{
		if (env->exported)
			n_exported++;
		env = env->next;
	}
	return (n_exported);
}

static void	fill_exported(const t_env *env, const t_env **arr)
{
	int	index;

	index = 0;
	while (env)
	{
		if (env->exported)
		{
			arr[index] = env;
			index++;
		}
		env = env->next;
	}
}

static void	swap_envptr(const t_env **a, const t_env **b)
{
	const t_env	*tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	sort_exported(const t_env **arr, int n)
{
	int	base_index;
	int	compare_index;

	base_index = 0;
	while (base_index < n)
	{
		compare_index= base_index + 1;
		while (compare_index < n)
		{
			if (ft_strcmp(arr[base_index]->key, arr[compare_index]->key) > 0)
				swap_envptr(&arr[base_index], &arr[compare_index]);
			compare_index++;
		}
		base_index++;
	}
}



static void	print_one_decl(const char *key, const char *value)
{
	size_t	i;

	write(1, "declare -x ", 11);
	write(1, key, ft_strlen(key));
	write(1, "=\"", 2);
	i = 0;
	while (value[i] != '\0')
	{
		if (value[i] == '\"' || value[i] == '\\')
			write(1, "\\", 1);
		write(1, &value[i], 1);
		i++;
	}
	write(1, "\"\n", 2);
}

void	print_export_sorted(const t_env *env)
{
	int			n;
	const t_env	**arr;
	int			index;

	n = count_exported(env);
	if (n <= 0)
		return ;
	arr = (const t_env **)malloc(sizeof(*arr) * n);
	if (arr == NULL)
		return ;
	fill_exported(env, arr);
	sort_exported(arr, n);
	index = 0;
	while (index < n)
	{
		print_one_decl(arr[index]->key, arr[index]->val);
		index++;
	}
	free((void *)arr);
}
