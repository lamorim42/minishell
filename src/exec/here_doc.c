/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/03 15:13:22 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/22 17:51:01 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

struct s_hd
{
	char	*content;
	char	*temp;
	char	*buffer;
};

static char	*put_nl_in_buf(char *temp, char *content);
static void	generation_content(t_pipe_list *node, struct s_hd *hd);

char	*here_doc_buffer(t_pipe_list *node)
{
	struct s_hd	hd;

	hd.buffer = NULL;
	hd.content = NULL;
	hd.temp = NULL;
	generation_content(node, &hd);
	if (hd.buffer != NULL)
		return (hd.buffer);
	return (hd.temp);
}

static void	generation_content(t_pipe_list *node, struct s_hd *hd)
{
	while (TRUE)
	{
		hd->content = readline("> ");
		if (hd->content == NULL)
		{
			error_msg(node->args[1], ": Wanted\n");
			break ;
		}
		if (hd->content && ft_strlen(hd->content) == ft_strlen(node->args[1])
			&& !ft_strncmp(hd->content, node->args[1],
				ft_strlen(node->args[1])))
		{
			free(hd->content);
			break ;
		}
		if (hd->temp == NULL)
		{
			hd->temp = char_cat(hd->content, '\n');
			free(hd->content);
		}
		else
			hd->buffer = put_nl_in_buf(hd->temp, hd->content);
	}
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

static char	*put_nl_in_buf(char *temp, char *content)
{
	char	*buffer;

	buffer = ft_strjoin(temp, content);
	free(temp);
	temp = buffer;
	buffer = char_cat(buffer, '\n');
	free(temp);
	free (content);
	temp = buffer;
	return (buffer);
}
