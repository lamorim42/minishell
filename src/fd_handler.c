/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_handler.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/18 14:54:40 by lamorim           #+#    #+#             */
/*   Updated: 2022/07/18 14:56:03 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	find_input(t_pipe_list *node)
{
	t_pipe_list	*temp;
	int			input;

	input = 0;
	temp = node->prev;
	if (temp && (!ft_strncmp(temp->args[0], "PIPE", 4)
			|| is_redirect_input(temp)))
		dup2(temp->fd[0], STDIN_FILENO);
	temp = node->next;
	while (temp)
	{
		if (!ft_strncmp(temp->args[0], "PIPE", 4))
			break ;
		if (is_redirect_input(temp))
			input = temp->fd[0];
		if (!file_exists(temp->fd[0]))
		{
			error_msg(temp->args[1], ": No such file or directory\n");
			return (-1);
		}
		temp = temp->next;
	}
	if (input > 0)
		dup2(input, STDIN_FILENO);
	return (input);
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
		if (is_redirect_output(temp))
		{
			output = temp->fd[0];
		}
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
	if (temp)
	{
		while (temp->prev)
			temp = temp->prev;
	}
	while (temp)
	{
		if ((is_redirect_output(temp) || !ft_strncmp(temp->args[0], "REDI", 4))
			&& file_exists(temp->fd[0]))
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

int	is_redirect_output(t_pipe_list *node)
{
	return (!ft_strncmp(node->args[0], "REDO", 4)
		|| !ft_strncmp(node->args[0], "REDA", 4));
}

int	is_redirect_input(t_pipe_list *node)
{
	return (!ft_strncmp(node->args[0], "REDI", 4)
		|| !ft_strncmp(node->args[0], "HERE", 4));
}
