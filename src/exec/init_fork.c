/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_fork.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:25:53 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/18 16:08:07 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	population_arr_pid(t_line *line, int pid);

void	init_fork(t_line *line, t_pipe_list *list, t_hash_table **table)
{
	int pid;

	pid = 0;

	if (list && (!ft_strncmp(list->args[0], "REDO", 4)
			|| !ft_strncmp(list->args[0], "REDA", 4)))
		close(list->fd[0]);
	else if (is_command(list))
	{
		signals_parent(line);
		pid = fork();
		if (pid == 0)
		{
			close_std_fd(line);
			signals_child(line);
			exec_path(line, list, table);
		}
		else
			population_arr_pid(line, pid);
	}
	if (list->prev && !ft_strncmp(list->prev->args[0], "PIPE", 4))
	{
		close(list->prev->fd[0]);
		close(list->prev->fd[1]);
	}
}

static void	population_arr_pid(t_line *line, int pid)
{
	int	i;

	i = 0;
	while (i < line->count_cmds)
	{
		if(line->pid[i] != 0)
		{
			i++;
			continue ;
		}
		line->pid[i] = pid;
		i++;
	}

	line->pid[i] = '\0';
}
