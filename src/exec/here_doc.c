/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/03 15:13:22 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/11 18:01:00 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*ft_eof(char *eof);

char	*here_doc_buffer(t_pipe_list *node)
{
	char	*content;
	char	*temp;
	char	*buffer;
	char	*eof;

	temp = NULL;
	content = NULL;
	buffer = NULL;
	eof = ft_eof(node->args[1]);
	while(1)
	{
		write(1, "> ", 2);
		content = get_next_line(1);
		if (content == NULL)
		{
			error_msg(eof, ": Wanted\n");
			break ;
		}
		if (content && ft_strcmp_len(content, eof))
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
	free(eof);
	if (buffer != NULL)
		return (buffer);
	else
		return (temp);
}

static char	*ft_eof(char *eof)
{
	char	*new_eof;

	new_eof = char_cat(eof, '\n');
	return (new_eof);
}