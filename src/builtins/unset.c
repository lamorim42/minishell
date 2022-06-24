/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:08:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/24 19:19:08 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	unset_builtin(t_pipe_list *node, t_hash_table **table)
{
	if (ft_strlen(node->args[0]) > 5)
	{
		printf("%s: command not found\n", node->args[0]);
		exit(127);
	}
	table_delete(table, node->args[1]);
}