/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/27 07:11:02 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/16 19:22:53 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	echo_builtin(t_pipe_list *node)
{
	int	i;

	i = 1;
	if (ft_strlen(node->args[0]) > 4)
	{
		error_msg(node->args[0], ": command not found\n");
		return ;
	}
	if (node->args[1] == NULL)
		ft_putstr_fd("\n", STDOUT_FILENO);
	if (node->args[1] != NULL && ft_strcmp_len(node->args[1], "-n"))
		i++;
	while (node->args[i])
	{
		if (node->args[i + 1] == NULL)
		{
			ft_putstr_fd(node->args[i], STDOUT_FILENO);
			break ;
		}
		if (node->args[i])
			ft_putstr_fd(node->args[i], STDOUT_FILENO);
		i++;
	}
	if ((node->args[1] != NULL) && (ft_strlen(node->args[1]) > 2 || ft_strncmp(node->args[1], "-n", 2) != 0))
		ft_putstr_fd("\n", STDOUT_FILENO);
}

// arrumar pipeline para builtins
