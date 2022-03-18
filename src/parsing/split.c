/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/18 09:28:34 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/18 10:23:42 by dmonteir         ###   ########.fr       */
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
	while (*ptr_len)
	{
		//printf("oii");
		// miau
		if (*ptr_len == 39 || *ptr_len == 34)
		{
			quot = *ptr_len;
			content = handle_quotes(&ptr_len, quot);
			printf("quots content: %s\n", content);
		}
		if (*ptr_len == 32 && (*(ptr_len + 1) != 39 && *(ptr_len + 1) != 34))
		{
			ptr_len++;
			content = handle_space(&ptr_len);
			printf("space content: %s\n", content);
		}
		else if (*ptr_len == line[0])
		{
			content = handle_first_arg(&ptr_len);
			printf("First_arg = %s\n", content);
		}
		add_back_stack(content, &(cmd_line->array));
		ptr_len++;
	}
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
			*ptr_len += (i - 1);
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
			*ptr_len += (i - 1);
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
