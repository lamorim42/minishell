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
	if (next && next->args
		&& (!ft_strncmp(next->args[0], "REDO", 4)
		|| !ft_strncmp(next->args[0], "REDA", 4)))
	{
		dup2(list->next->fd[0], STDOUT_FILENO);
	}
	if (ft_strncmp(list->bin, "builtin", 7) == 0)
	{
		exec_builtins(list);
		exit(0);
	}
	else if (!execve(list->bin, list->args, line->envp))
	{
		free_line(line);
		exit(1);
	}
}
