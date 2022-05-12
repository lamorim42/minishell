/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list_commands.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 19:19:07 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/11 20:49:46 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	list_generation_bin(t_line *line)
{
	t_pipe_list *temp;

	temp = line->list_cmds;
	while (temp)
	{
		if (temp->args && ft_strncmp(temp->args[0], "PIPE", 4))
		{
			temp->bin = path_finder(line, temp->args[0]);
		}
		temp = temp->next;
	}
}

void	exec_list(t_line *line)
{
	t_pipe_list *temp;

	temp = line->list_cmds;
	while (temp)
	{
		if (ft_strncmp(temp->args[0], "PIPE", 4))
		{
			init_fork(line, temp);
		}
		temp = temp->next;
	}
}