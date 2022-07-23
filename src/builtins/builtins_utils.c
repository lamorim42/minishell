/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/22 13:58:12 by lamorim           #+#    #+#             */
/*   Updated: 2022/07/23 15:23:01 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern struct s_minishell	g_minishell;

void	exec_builtins(t_pipe_list *node, t_hash_table **table)
{
	if (ft_strcmp_len(node->args[0], "echo"))
		echo_builtin(node);
	else if (ft_strcmp_len(node->args[0], "cd"))
		cd_builtin(node, table);
	else if (ft_strcmp_len(node->args[0], "pwd"))
		pwd_builtin(table);
	else if (ft_strcmp_len(node->args[0], "export"))
		export_builtin(node, table);
	else if (ft_strcmp_len(node->args[0], "unset"))
		unset_builtin(node, table);
	else if (ft_strcmp_len(node->args[0], "env"))
		env_builtin(node, table);
	else if (ft_strcmp_len(node->args[0], "exit"))
		exit_builtin(node, table);
	else
	{
		g_minishell.line->status_code = 127;
		error_msg(node->args[0], ": command not found\n");
	}
}

int	is_a_builtin(char **node)
{
	if (node[0][0] != '\0' && (ft_strncmp(*node, "echo", 4) == 0
		|| ft_strncmp(*node, "cd", 2) == 0
		|| ft_strncmp(*node, "pwd", 3) == 0
		|| ft_strncmp(*node, "export", 6) == 0
		|| ft_strncmp(*node, "unset", 5) == 0
		|| ft_strncmp(*node, "env", 3) == 0
		|| ft_strncmp(*node, "exit", 4) == 0))
		return (FALSE);
	return (TRUE);
}

int	is_args_ok(char *arg)
{
	if (ft_isalpha(arg[0]))
		return (TRUE);
	return (FALSE);
}

void	insert_var_in_hash(char *arg, t_hash_table **table)
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
		insert_simple_var_in_hash(table, split_str);
	ft_free_arr(split_str);
}

void	insert_simple_var_in_hash(t_hash_table **table, char **split_str)
{
	char	*temp;

	temp = search_item(*table, split_str[0]);
	if (temp == NULL)
	{
		hash_insert(table, split_str[0], split_str[1]);
		free(temp);
	}
	else
	{
		update_var_env(table, split_str[0], split_str[1]);
		free(temp);
	}
}
