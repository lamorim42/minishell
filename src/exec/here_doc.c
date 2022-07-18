/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/03 15:13:22 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/18 15:57:50 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*here_doc_buffer(t_pipe_list *node)
{
	char	*content;
	char	*temp;
	char	*buffer;

	temp = NULL;
	content = NULL;
	buffer = NULL;
	while(1)
	{
		content = readline("> ");
		if (content == NULL)
		{
			error_msg(node->args[1], ": Wanted\n");
			break ;
		}
		if (content && ft_strlen(content) == ft_strlen(node->args[1])
			&& !ft_strncmp(content, node->args[1], ft_strlen(node->args[1])))
			{
				free(content);
				break ;
			}
		if (temp == NULL)
		{
			temp = char_cat(content, '\n');
			free(content);
		}
		else
		{
			buffer = ft_strjoin(temp, content);
			free(temp);
			temp = buffer;
			buffer = char_cat(buffer, '\n');
			free(temp);
			temp = buffer;
			free (content);
		}
	}
	if (buffer != NULL)
		return (buffer);
	else
		return (temp);
}

void	here_doc_verification(t_line *line)
{
	t_pipe_list *temp;
	char	*buffer;

	temp = line->list_cmds;
	buffer = NULL;
	while(temp)
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
