/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/27 07:11:02 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/28 19:45:58 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	echo_builtin(t_pipe_list *node)
{
	int i;
	i = 1;
	while (node)
	{
		if (node->args[i] == NULL)
			ft_putchar_fd('\n', 2);
		if (node->args[i])
		{
			ft_putstr_fd(node->args[i], 2);
			ft_putchar_fd('\n', 2);
		}
		node = node->next;
	}
}