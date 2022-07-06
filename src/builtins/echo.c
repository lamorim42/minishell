/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/27 07:11:02 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/05 20:04:16 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	echo_builtin(t_pipe_list *node)
{
	int	i;

	i = 1;
	if (ft_strlen(node->args[0]) > 4)
	{
		printf("%s: command not found\n", node->args[0]);
		exit(127);
	}
	if (node->args[1] == NULL)
		printf("\n");
	if (ft_strncmp(node->args[1], "-n", 2) == 0 && ft_strlen(node->args[1]) == 2)
		i++;
	while (node->args[i])
	{
		if (node->args[i + 1] == NULL)
		{
			printf("%s", node->args[i]);
			break ;
		}
		if (node->args[i])
			printf("%s ", node->args[i]);
		i++;
	}
	if (ft_strlen(node->args[1]) > 2 || ft_strncmp(node->args[1], "-n", 2) != 0)
		printf("\n");
}
