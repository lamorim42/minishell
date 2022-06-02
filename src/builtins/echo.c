/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/27 07:11:02 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/31 20:42:13 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	echo_builtin(t_pipe_list *node)
{
	int i;
	i = 1;
	while (node->args[i + 1])
	{
		if (node->args[i] == NULL)
			printf("\n");
		if (node->args[i])
		{
			printf("%s ", node->args[i]);
		}
		i++;
	}
	printf("%s", node->args[i]);
	printf("\n");
}
