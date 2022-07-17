/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:08:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/13 19:50:28 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	unset_builtin(t_pipe_list *node, t_hash_table **table)
{
	char *value;

	if (ft_strlen(node->args[0]) > 5)
	{
		error_msg(node->args[0], "command not found\n");
		return ;
	}
	if (ft_isalpha(node->args[1][0]))
	{
		value = search_item(*table, node->args[1]);
		if (value != NULL)
		{
			table_delete(table, node->args[1]);
			free(value);
		}
		else
		{
			free(value);
			return ;
		}
	}
	else
		error_msg(node->args[1], ": not a valid identifier\n");
}