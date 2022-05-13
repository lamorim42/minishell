/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_fork.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:25:53 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/13 18:34:10 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_fork(t_line *line, t_pipe_list *list)
{
	if (list->next && !ft_strncmp(list->next->args[0], "PIPE", 4))
	{
		if (pipe(list->next->fd) != 0)
			dprintf(2, "pipe error\n");
	}

	line->pid = fork();
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