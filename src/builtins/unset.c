/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:08:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/18 21:44:51 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	unset_builtin(t_pipe_list *node, t_hash_table **table)
{
	char *value;

	if (node->args[1] != NULL && ft_isalpha(node->args[1][0]))
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
	else if (node->args[1])
	{
		g_minishell.line->status_code = 1;
		error_msg(node->args[1], ": not a valid identifier\n");
	}
}