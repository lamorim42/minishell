/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:08:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 16:15:00 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern struct s_minishell	g_minishell;

void	unset_builtin(t_pipe_list *node, t_hash_table **table)
{
	char	*value;
	int		i;

	i = 1;
	while (node->args[i])
	{
		if (ft_isalpha(node->args[i][0]))
		{
			value = search_item(*table, node->args[i]);
			if (value != NULL)
			{
				table_delete(table, node->args[i]);
				free(value);
			}
			else
				free(value);
		}
		else if (node->args[i])
		{
			g_minishell.line->status_code = 1;
			error_msg(node->args[i], ": not a valid identifier\n");
		}
		i++;
	}
}
