/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/03 15:13:22 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 10:55:13 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

struct s_hd
{
	char	*content;
	char	*temp;
	char	*buffer;
};

static void	putting_nl(struct s_hd *hd);
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
		putting_nl(hd);
	}
}

static void	putting_nl(struct s_hd *hd)
{
	if (!hd->temp)
	{
		hd->temp = char_cat(hd->content, '\n');
		free(hd->content);
	}
	else
	{
		hd->buffer = put_nl_in_buf(hd->temp, hd->content);
		hd->temp = hd->buffer;
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
	return (buffer);
}
