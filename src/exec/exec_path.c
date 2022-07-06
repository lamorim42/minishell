/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:04:40 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/06 20:14:27 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	find_input(t_pipe_list *node);
void	find_output(t_pipe_list *node);

void	exec_path(t_line *line, t_pipe_list *list, t_hash_table **table)
{
	find_input(list);
	find_output(list);
	close_fds(list);
	if (!list->bin)
	{
		error_msg(list->args[0], ": command not found\n");
		exit(127);
	}
	else if (ft_strncmp(list->bin, "builtin", 7) == 0)
	{
		exec_builtins(list, table);
		exit(0);
	}
	else if (execve(list->bin, list->args, line->envp) == -1)
	{
		free_line(line);
		exit(127);
	}
}

void	error_msg(char *str, char *msg)
{
	char	*buffer;

	buffer = ft_strjoin(str, msg);
	ft_putstr_fd(buffer, 2);
	free(buffer);
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
