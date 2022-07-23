/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:09:08 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 15:23:19 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern struct s_minishell	g_minishell;

void	env_builtin(t_pipe_list *node, t_hash_table **table)
{
	if (node->args[1] != NULL)
	{
		g_minishell.line->status_code = 127;
		error_msg(node->args[1], ": No such file or directory\n");
		return ;
	}
	print_table(table);
}
