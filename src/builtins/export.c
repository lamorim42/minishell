/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:07:29 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/16 08:09:09 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_args_ok(char *arg);
int			count_args(t_pipe_list *node);
void		insert_var_in_hash(char *arg, t_hash_table **table);

void	export_builtin(t_pipe_list *node, t_hash_table **table)
{
	int	i;

	i = 1;
	if (ft_strlen(node->args[0]) > 6)
	{
		error_msg(node->args[0], ": commmand not found\n");
		return ;
	}

	while (node->args[i])
	{
		if (is_args_ok(node->args[i]) == 1 && ft_strchr(node->args[i], '='))
		{
			insert_var_in_hash(node->args[i], table);
		}
		else if (is_args_ok(node->args[i]) == 0)
			error_msg(node->args[i], ": not a valid identifier\n");
		i++;
	}
}

static int	is_args_ok(char *arg)
{
	if (ft_isalpha(arg[0]))
		return (1);
	return (0);
}

void	insert_var_in_hash(char *arg, t_hash_table **table)
{
	char	**split_str;
	int		len;
	int		i;
	char	*temp;

	i = 1;
	temp = NULL;
	split_str = NULL;
	split_str = ft_split(arg, '=');
	len = ft_array_len(split_str);
	if (len > 2)
	{
		while(split_str[i])
		{
			if (i % 2 != 0)
			{
				temp = ft_strjoin(split_str[i], "=");
				free(split_str[1]);
				split_str[1] = ft_strdup(temp);
				free(temp);
			}
			else
			{
				temp = ft_strjoin(split_str[1], split_str[i]);
				free(split_str[1]);
				split_str[1] = ft_strdup(temp);
				free(temp);
			}
			i++;
		}
	}
	if (split_str[1])
		hash_insert(table, split_str[0], split_str[1]);
	ft_free_arr(split_str);
}
