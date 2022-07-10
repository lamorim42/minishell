/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:09:08 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/10 12:39:01 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	env_builtin(t_pipe_list *node, t_hash_table **table)
{
	if (ft_strlen(node->args[0]) > 3)
	{
		error_msg(node->args[0], ": command not found\n");
		return ;
	}
	if (node->args[1] != NULL)
	{
		error_msg(node->args[0], ": No such file or directory\n");
		return ;
	}
	print_table(table);
}
