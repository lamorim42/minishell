/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/27 07:11:02 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 14:51:33 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	echo_builtin(t_pipe_list *node)
{
	int	i;

	i = 1;
	if (node->args[1] == NULL)
		printf("\n");
	while (node->args[i] != NULL && ft_strcmp_len(node->args[i], "-n"))
		i++;
	while (node->args[i])
	{
		if (node->args[i][0] != '\0' && node->args[i + 1] == NULL)
		{
			printf("%s", node->args[i]);
			break ;
		}
		if (node->args[i] && node->args[i][0] != '\0')
			printf("%s ", node->args[i]);
		i++;
	}
	if ((node->args[1] != NULL) && !ft_strcmp_len(node->args[1], "-n"))
		printf("\n");
}
