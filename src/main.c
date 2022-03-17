/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 08:29:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/17 13:48:47by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	parse(char *line);
char	*split_line(char *line);
char	*worddup(const char *s, size_t len);
int		has_double_quotation(char *str, char quot);
char	*handle_quotes(char **ptr_len, char quot);
void	error(void);

int	main(void)
{
	char *line;

	line = readline("miau> ");


	parse(line);
	free(line);

	return (0);
}

char	*split_line(char *line)
{
	char	*ptr_len;
	char	*content;
	char	quot;

	ptr_len = line;
	while (*ptr_len)
	{
		if (*ptr_len == 39 || *ptr_len == 34)
		{
			quot = *ptr_len;
			content = handle_quotes(&ptr_len, quot);
		}
		ptr_len++;
	}
	return (content);
}

char	*handle_quotes(char **ptr_len, char quot)
{
	int	i;
	char	*content;

	i = 0;
	content = NULL;
	if (has_double_quotation((*ptr_len), **ptr_len) == 1)
	{
		(*ptr_len)++;
		while ((*ptr_len)[i])
		{
			if ((*ptr_len)[i] == quot)
			{
				content = worddup((*ptr_len), i);
				(*ptr_len) += i;
				break ;
			}
			i++;
		}
	}
	else
		error();
	return (content);
}

// "main function"

int	has_double_quotation(char *str, char quot)
{
	int	ret;
	int	count;

	ret = 0;
	count = 0;
	//printf("str = %s\n", str);
	while (*str)
	{
		if (*str == quot)
			count++;
		str++;
	}
	if (count % 2 == 0)
		ret = 1;
	return (ret);
}

char	*worddup(const char *s, size_t len)
{
	char	*str;
	size_t	offset;

	str = malloc(len + 1);
	if (str == NULL)
		error();
	offset = 0;
	while (offset < len)
	{
		str[offset] = s[offset];
		offset++;
	}
	str[offset] = '\0';
	return (str);
}

void	parse(char *line)
{
	size_t	i;
	char	*word;

	i = 0;
	word = split_line(line);
	printf("%s\n", word);
	while (i < strlen(line))
	{
		if (strchr(SEP, line[i]))
		{
			printf("oiii\n");
		}
		i++;
	}
}

void	error(void)
{
	perror("Error!");
	exit(1);
}
