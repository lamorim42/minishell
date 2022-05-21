/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_fork.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:25:53 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/21 17:08:24 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_fork(t_line *line, t_pipe_list *list)
{
	// ls | grep file
	if (list->next && !ft_strncmp(list->next->args[0], "PIPE", 4))
	{
		if (pipe(list->next->fd) != 0)
			dprintf(2, "pipe error\n");
	}
	else if (list->next && !ft_strncmp(list->next->args[0], "REDO", 4))
	{
		list->next->fd[0] = open(list->next->args[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (list->next->fd[0] == -1)
		{
			perror(list->next->args[1]);
			free_list(list);
			exit(1);
		}
	}

	line->pid = fork();
	if (list && !ft_strncmp(list->args[0], "REDO", 4))
		close(list->fd[0]);
	if (line->pid == 0)
	{
		exec_path(line, list);
	}
	if (list->prev && !ft_strncmp(list->prev->args[0], "PIPE", 4))
	{
		close(list->prev->fd[0]);
		close(list->prev->fd[1]);
	}
	waitpid(line->pid, NULL, 0);
}