/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 19:17:50 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/22 15:41:19 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	cd_builtin(t_pipe_list *node, t_hash_table **table)
{
	char	*str_pwd;

	str_pwd = NULL;
	//str_pwd = print_search(table, "PWD");

	/* str_pwd = node->args[1];
	table_delete(table, "PWD");
	hash_insert(table, "PWD", str_pwd); */

	if (chdir(node->args[1]) == -1)
	{
		printf("bash: cd: %s: No such file or directory\n", node->args[1]);
		exit(1);
	}
}
