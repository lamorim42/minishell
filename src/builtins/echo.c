/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/27 07:11:02 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/18 21:02:38 by lamorim          ###   ########.fr       */
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
		if (node->args[i + 1] == NULL)
		{
			printf("%s", node->args[i]);
			break ;
		}
		if (node->args[i])
			printf("%s ", node->args[i]);
		i++;
	}
	if ((node->args[1] != NULL) && (ft_strlen(node->args[1]) > 2 || ft_strncmp(node->args[1], "-n", 2) != 0))
		printf("\n");
}
