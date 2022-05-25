/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list_commands.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 19:19:07 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/21 16:2 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	list_generation_bin(t_line *line)
{
	t_pipe_list *temp;

	temp = line->list_cmds;
	while (temp)
	{
		if (temp->args && temp->args[0] && ft_strncmp(temp->args[0], "PIPE", 4)
			&& ft_strncmp(temp->args[0], "REDO", 4)
			&& ft_strncmp(temp->args[0], "REDA", 4))
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
		if (!temp->prev && (!ft_strncmp(temp->args[0], "REDO", 4)
			|| !ft_strncmp(temp->args[0], "REDA", 4)))
		{
			temp->fd[0] = open(temp->args[1], O_WRONLY | O_CREAT |
			(O_TRUNC * (ft_strncmp(temp->args[0], "REDO", 4) == 0)) |
			(O_APPEND * ft_strncmp(temp->args[0], "REDA", 4) == 0), 0644);
			if (temp->fd[0] == -1)
			{
				perror(temp->args[1]);
				free_line(line);
			}
			close(temp->fd[0]);
		}
		init_fork(line, temp);
		temp = temp->next;
	}
}