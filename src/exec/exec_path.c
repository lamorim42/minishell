/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:04:40 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/02 18:27:53 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	dup_fd(t_pipe_list *list);

void	exec_path(t_line *line, t_pipe_list *list)
{
	dup_fd(list);
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

static void	dup_fd(t_pipe_list *list)
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
}
