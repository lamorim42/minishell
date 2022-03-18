/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/18 09:28:34 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/18 19:37:48 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

char	*split_line(char *line)
{
	t_stack	*cmd_line;
	char	*ptr_len;
	char	*content;
	char	quot;

	ptr_len = line;
	cmd_line = malloc(sizeof(t_stack));
	cmd_line->array = NULL;
	content = handle_first_arg(&ptr_len);
	cmd_line->array = new_node(content);
	while (*ptr_len)
	{
		if (*ptr_len == 39 || *ptr_len == 34)
		{
			quot = *ptr_len;
			content = handle_quotes(&ptr_len, quot);
			add_back_stack(content, &(cmd_line->array));
		}
		else if (*ptr_len == 32 && (*(ptr_len + 1) != 39 && *(ptr_len + 1) != 34))
		{
			ptr_len++;
			content = handle_space(&ptr_len);
			add_back_stack(content, &(cmd_line->array));
		}
		else
			ptr_len++;
	}
	printf("-------\n");
	print_list(&cmd_line);
	return (content);
}

char	*handle_first_arg(char **ptr_len)
{
	int	i;
	char	*content;

	i = 0;
	content = NULL;
	while((*ptr_len)[i])
	{
		if ((*ptr_len)[i] == 32 || (*ptr_len)[i + 1] == '\0')
		{
			content = worddup(*ptr_len, i + 1);
			*ptr_len += (i);
			break ;
		}
		i++;
	}
	return (content);
}

char	*handle_space(char **ptr_len)
{
	int		i;
	char	*content;

	i = 0;
	content = NULL;
	while ((*ptr_len)[i])
	{
		if ((*ptr_len)[i] == 32 || (*ptr_len)[i + 1] == '\0')
		{
			content = worddup(*ptr_len, i + 1);
			*ptr_len += (i);
			break ;
		}
		i++;
	}
	return (content);
}

char	*worddup(const char *s, size_t len)
{
	char	*str;
	size_t	offset;

	str = malloc(len + 1);
	if (str == NULL)
		return_error();
	offset = 0;
	while (offset < len)
	{
		str[offset] = s[offset];
		offset++;
	}
	str[offset] = '\0';
	return (str);
}
