/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_export_sorted.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 16:39:14 by takawagu          #+#    #+#             */
/*   Updated: 2025/10/24 17:26:06 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_exported(const t_env *env)
{
	int	n;

	n = 0;
	while (env)
	{
		if (env->exported)
			++n;
		env = env->next;
	}
	return (n);
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

static void	sort_exported(const t_env **arr, int n)
{
	int	a;
	int	b;

	a = 0;
	while (a < n)
	{
		b = a + 1;
		while (b < n)
		{
			if (ft_strcmp(arr[a]->key, arr[b]->key) > 0)
				swap_envptr(&arr[a], &arr[b]);
			++b;
		}
		++a;
	}
}

static void	swap_envptr(const t_env **a, const t_env **b)
{
	const t_env	*tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	print_one_decl(const char *k, const char *v)
{
	size_t	i;

	write(1, "declare -x ", 11);
	write(1, k, ft_strlen(k));
	write(1, "=\"", 2);
	i = 0;
	while (v[i] != '\0')
	{
		if (v[i] == '\"' || v[i] == '\\')
			write(1, "\\", 1);
		write(1, &v[i], 1);
		++i;
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
