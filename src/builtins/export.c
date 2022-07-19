/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:07:29 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/19 15:52:37 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_args_ok(char *arg);
static void	building_var(char **split, char *str, char *s);
static void	insert_var_in_hash(char *arg, t_hash_table **table);
static void	print_export(t_hash_table **table);

struct s_export
{
	t_hash_item	*current;
	t_hash_item	*temp;
	int			i;
	int			counter;
};

void	export_builtin(t_pipe_list *node, t_hash_table **table)
{
	int	i;

	i = 1;
	while (node->args[i])
	{
		if (is_args_ok(node->args[i]) == 1 && ft_strchr(node->args[i], '='))
			insert_var_in_hash(node->args[i], table);
		else if (is_args_ok(node->args[i]) == 0)
		{
			g_minishell.line->status_code = 1;
			error_msg(node->args[i], ": not a valid identifier\n");
		}
		i++;
	}
	if (!node->args[1])
		print_export(table);
}

static int	is_args_ok(char *arg)
{
	if (ft_isalpha(arg[0]))
		return (1);
	return (0);
}

static void	insert_var_in_hash(char *arg, t_hash_table **table)
{
	char	**split_str;
	int		len;
	int		i;

	i = 1;
	split_str = NULL;
	split_str = ft_split(arg, '=');
	len = ft_array_len(split_str);
	if (len > 2)
	{
		while (split_str[i])
		{
			if (i % 2 != 0)
				building_var(split_str, split_str[i], "=");
			else
				building_var(split_str, split_str[1], split_str[1]);
			i++;
		}
	}
	if (split_str[1])
		hash_insert(table, split_str[0], split_str[1]);
	ft_free_arr(split_str);
}

static void	building_var(char **split, char *str, char *s)
{
	char	*temp;

	temp = ft_strjoin(str, s);
	free(split[1]);
	split[1] = ft_strdup(temp);
	free(temp);
}

static void	print_export(t_hash_table **table)
{
	struct s_export	var;

	var.current = NULL;
	var.i = 0;
	var.counter = 0;
	while (var.i < (*table)->size)
	{
		if ((*table)->item[var.i])
		{
			var.current = (*table)->item[var.i];
			while (var.current != NULL)
			{
				var.temp = var.current->next;
				if (var.current->key != NULL)
				{
					printf("declare -x ");
					printf("%s=\"%s\"\n", var.current->key, var.current->value);
				}
				var.current = var.temp;
				var.counter++;
			}
			var.counter++;
		}
		var.i++;
	}
}
