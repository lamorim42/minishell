/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:09:08 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/20 21:12:50 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	env_builtin(t_pipe_list *node, t_hash_table **table)
{
	if (ft_strlen(node->args[0]) > 3)
	{
		printf("Command '%s' not found\n", node->args[0]);
		exit(127);
	}
	if (node->args[1] != NULL)
	{
		printf("env: '%s': No such file or directory\n", node->args[1]);
		exit(127);
	}
	print_table(table);
}
