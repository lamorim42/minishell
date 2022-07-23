/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:07:29 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 15:23:35 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern struct s_minishell	g_minishell;

struct s_export
{
	t_hash_item	*current;
	t_hash_item	*temp;
	int			i;
	int			counter;
};

static void	print_export(t_hash_table **table);
static void	print_var_line(struct s_export *var);

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

void	building_var(char **split, char *str, char *s)
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
			print_var_line(&var);
			var.counter++;
		}
		var.i++;
	}
}

static void	print_var_line(struct s_export *var)
{
	while (var->current != NULL)
	{
		var->temp = var->current->next;
		if (var->current->key != NULL)
		{
			printf("declare -x ");
			printf("%s=\"%s\"\n", var->current->key, var->current->value);
		}
		var->current = var->temp;
		var->counter++;
	}
}
