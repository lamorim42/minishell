/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:04:40 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/24 19:53:19 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	dup_fd(t_pipe_list *list);

void	exec_path(t_line *line, t_pipe_list *list, t_hash_table **table)
{
	verification_input(list);
	dup_fd(list);
	if (ft_strncmp(list->bin, "builtin", 7) == 0)
			exec_builtins(list, table);
	if (!execve(list->bin, list->args, line->envp))
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
		close(list->next->fd[0]);
	}
}

void	verification_input(t_pipe_list *temp)
{
	if (temp->prev && !ft_strncmp(temp->prev->args[0], "REDI", 4))
	{
		temp->prev->fd[0] = open(temp->prev->args[1], O_RDONLY);
		dup2(temp->prev->fd[0], STDIN_FILENO);
		close(temp->prev->fd[0]);
	}
	else if (temp->next && !ft_strncmp(temp->next->args[0], "REDI", 4))
	{
		temp->next->fd[0] = open(temp->next->args[1], O_RDONLY);
		dup2(temp->next->fd[0], STDIN_FILENO);
		close(temp->next->fd[0]);
	}
}

