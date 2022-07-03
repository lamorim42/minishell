/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:07:29 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/24 19:05:45 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	export_builtin(t_pipe_list *node, t_hash_table **table)
{
	char **split_str;

	if (ft_strlen(node->args[0]) > 6)
	{
		printf("%s: command not found\n", node->args[0]);
		exit(127);
	}
	split_str = ft_split(node->args[1], '=');
	if (split_str[0] != NULL && split_str[1] != NULL)
		hash_insert(table, split_str[0], split_str[1]);
}
