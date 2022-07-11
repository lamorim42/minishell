/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:08:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/11 18:32:28 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	unset_builtin(t_pipe_list *node, t_hash_table **table)
{
	if (ft_strlen(node->args[0]) > 5)
	{
		error_msg(node->args[0], "command not found\n");
		return ;
	}
	if (ft_isalpha(node->args[1][0]))
		table_delete(table, node->args[1]);
	else
		error_msg(node->args[1], ": not a valid identifier\n");
}