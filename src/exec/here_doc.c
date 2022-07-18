/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/03 15:13:22 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/17 17:38:51 by dmonteir         ###   ########.fr       */
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
