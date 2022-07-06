/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:04:40 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/06 17:23:04 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//static void	dup_fd(t_pipe_list *list);
void	find_input(t_pipe_list *node);
void	find_output(t_pipe_list *node);

void	exec_path(t_line *line, t_pipe_list *list, t_hash_table **table)
{
	//verification_input(list);
	//dup_fd(list);
	find_input(list);
	find_output(list);
	close_fds(list);
	if (list->bin && ft_strncmp(list->bin, "builtin", 7) == 0)
	{
		exec_builtins(list, table);
		exit(0);
	}
	if (execve(list->bin, list->args, line->envp) == -1)
	{
		free_line(line);
		exit(127);
	}
}

void	find_input(t_pipe_list *node)
{
	t_pipe_list	*temp;
	int			input;

	input = 0;
	temp = node->prev;
	if (temp && (!ft_strncmp(temp->args[0], "PIPE", 4)
		|| !ft_strncmp(temp->args[0], "REDI", 4)
		|| !ft_strncmp(temp->args[0], "HERE", 4)))
		dup2(temp->fd[0], STDIN_FILENO);
	temp = node->next;
	while (temp)
	{
		if (!ft_strncmp(temp->args[0], "PIPE", 4))
			break ;
		if (!ft_strncmp(temp->args[0], "REDI", 4)
		|| !ft_strncmp(temp->args[0], "HERE", 4))
			input = temp->fd[0];
		temp = temp->next;
	}
	if (input != 0)
		dup2(input, STDIN_FILENO);
}

void	find_output(t_pipe_list *node)
{
	t_pipe_list	*temp;
	int			output;

	output = 1;
	temp = node->next;
	while (temp)
	{
		if (!ft_strncmp(temp->args[0], "PIPE", 4))
		{
			if (output == 1)
				output = temp->fd[1];
			break ;
		}
		if (!ft_strncmp(temp->args[0], "REDO", 4)
		|| !ft_strncmp(temp->args[0], "REDA", 4))
			output = temp->fd[0];
		temp = temp->next;
	}
	if (output != 1)
	{
		dup2(output, STDOUT_FILENO);
		close(output);
	}
}

void	close_fds(t_pipe_list *node)
{
	t_pipe_list	*temp;

	temp = node;
	while (temp->prev)
		temp = temp->prev;
	while (temp)
	{
		if (!ft_strncmp(temp->args[0], "REDO", 4)
		|| !ft_strncmp(temp->args[0], "REDA", 4)
		|| !ft_strncmp(temp->args[0], "REDI", 4))
			close(temp->fd[0]);
		else if (!ft_strncmp(temp->args[0], "PIPE", 4)
			|| !ft_strncmp(temp->args[0], "HERE", 4))
		{
			close(temp->fd[0]);
			close(temp->fd[1]);
		}
		temp = temp->next;
	}
}

// static void	dup_fd(t_pipe_list *list)
// {
// 	t_pipe_list	*next;
// 	t_pipe_list	*prev;

// 	next = list->next;
// 	prev = list->prev;
// 	if (next && next->args && !ft_strncmp(next->args[0], "PIPE", 4))
// 	{
// 		dup2(list->next->fd[1], STDOUT_FILENO);
// 		close(list->next->fd[0]);
// 		close(list->next->fd[1]);
// 	}
// 	if (prev && prev->args && !ft_strncmp(list->prev->args[0], "PIPE", 4))
// 	{
// 		dup2(list->prev->fd[0], STDIN_FILENO);
// 		close(list->prev->fd[1]);
// 		close(list->prev->fd[0]);

// 		if (next && !ft_strncmp(next->args[0], "HERE", 4))
// 		{
// 			dup2(next->fd[0], STDIN_FILENO);
// 			close(next->fd[0]);
// 		}
// 	}
// 	if (next && next->args
// 		&& (!ft_strncmp(next->args[0], "REDO", 4)
// 			|| !ft_strncmp(next->args[0], "REDA", 4)))
// 	{
// 		dup2(list->next->fd[0], STDOUT_FILENO);
// 		close(list->next->fd[0]);
// 	}
// }

// void	verification_input(t_pipe_list *temp)
// {
// 	if (temp->prev && !ft_strncmp(temp->prev->args[0], "REDI", 4))
// 	{
// 		temp->prev->fd[0] = open(temp->prev->args[1], O_RDONLY);
// 		dup2(temp->prev->fd[0], STDIN_FILENO);
// 		close(temp->prev->fd[0]);
// 	}
// 	else if (temp->next && !ft_strncmp(temp->next->args[0], "REDI", 4))
// 	{
// 		temp->next->fd[0] = open(temp->next->args[1], O_RDONLY);
// 		dup2(temp->next->fd[0], STDIN_FILENO);
// 		close(temp->next->fd[0]);
// 	}

// 	if ((temp->next && temp->next->next && !ft_strncmp(temp->next->next->args[0], "PIPE", 4)) || (temp->prev == NULL && temp->next->next == NULL))
// 	{
// 		if (temp->prev && !ft_strncmp(temp->prev->args[0], "HERE", 4))
// 		{
// 			dup2(temp->prev->fd[0], STDIN_FILENO);
// 			close(temp->prev->fd[0]);
// 		}
// 		else if (temp->next && !ft_strncmp(temp->next->args[0], "HERE", 4))
// 		{
// 			dup2(temp->next->fd[0], STDIN_FILENO);
// 			close(temp->next->fd[0]);
// 		}
// 	}
// }

//bug do cat << eof | ls onde não ta mandando pro pipe
