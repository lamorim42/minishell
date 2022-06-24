/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 07:38:27 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/24 19:06:23 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	pwd_builtin(t_pipe_list *node, t_hash_table **table)
{
	char	*str_pwd;

	if (ft_strlen(node->args[0]) > 3)
	{
		printf("%s: command not found\n", node->args[0]);
		exit(127);
	}
	str_pwd = search_item(table, "PWD");
	printf("%s\n", str_pwd);
}
