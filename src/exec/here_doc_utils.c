/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/22 15:02:04 by lamorim           #+#    #+#             */
/*   Updated: 2022/07/23 10:49:56 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	here_doc_verification(t_line *line)
{
	t_pipe_list	*temp;
	char		*buffer;

	temp = line->list_cmds;
	buffer = NULL;
	while (temp)
	{
		if (temp->args[0][0] != '\0' && ft_strcmp_len(temp->args[0], "HERE"))
		{
			buffer = here_doc_buffer(temp);
			here_doc_write(buffer, temp);
			free(buffer);
		}
		temp = temp->next;
	}
}

void	here_doc_write(char *buffer, t_pipe_list *node)
{
	if (buffer)
		write(node->fd[1], buffer, ft_strlen(buffer));
	close(node->fd[0]);
	close(node->fd[1]);
}

void	creat_here_doc(t_pipe_list *list)
{
	t_pipe_list	*node;

	node = list;
	while (node)
	{
		if (ft_strcmp_len(node->args[0], "HERE"))
			if (pipe(node->fd) != 0)
				perror("pipe()");
		node = node->next;
	}
}

void	close_here_doc(t_pipe_list *list)
{
	t_pipe_list	*node;

	node = list;
	while (node)
	{
		if (ft_strcmp_len(node->args[0], "HERE"))
			close(node->fd[1]);
		node = node->next;
	}
}
