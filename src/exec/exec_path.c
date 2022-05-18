/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:04:40 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/11 21:36:52by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exec_path(t_line *line, t_pipe_list *list)
{
	// ls | grep file
	t_pipe_list	*next;
	t_pipe_list	*prev;

	next = list->next;
	prev = list->prev;
	if (next && next->args && !ft_strncmp(next->args[0], "PIPE", 4))
	{
		dup2(list->next->fd[1], STDOUT_FILENO);
		close(list->next->fd[0]);
		close(list->next->fd[1]);
	}
	if (prev && prev->args && !ft_strncmp(list->prev->args[0], "PIPE", 4))
	{
		dup2(list->prev->fd[0], STDIN_FILENO);
		close(list->prev->fd[1]);
		close(list->prev->fd[0]);
	}
	//print_array("cmds", line->ctks)

	if (!execve(list->bin, list->args, line->envp))
		free_line(line);
}
