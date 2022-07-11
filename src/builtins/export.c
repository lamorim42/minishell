/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:07:29 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/11 18:35:49 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	export_builtin(t_pipe_list *node, t_hash_table **table)
{
	char **split_str;

	if (ft_strlen(node->args[0]) > 6)
	{
		error_msg(node->args[0], ": commmand not found\n");
		return ;
	}
	if (ft_isalpha(node->args[1][0]))
	{
		split_str = ft_split(node->args[1], '=');
		if (split_str[0] != NULL && split_str[1] != NULL)
			hash_insert(table, split_str[0], split_str[1]);
	}
	else
		error_msg(node->args[1], ": not a valid identifier\n");
}
