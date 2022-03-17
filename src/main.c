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
void	split_line(char *line);
char	*worddup(const char *s, size_t len);
int		has_double_quotation(char *str, char quot);

int	main(void)
{
	char *line;

	line = readline("miau> ");
	split_line(line);

	//parse(line);
	free(line);

	return (0);
}

void	split_line(char *line)
{
	char	*ptr_len;
	size_t	i;
	char	*content;
	char	quot;

	ptr_len = line;
	while (*ptr_len)
	{
		if (*ptr_len == 39 || *ptr_len == 34)
		{
			quot = *ptr_len;
			i = 0;
			if (has_double_quotation(ptr_len, *(ptr_len)) == 1)
			{
				ptr_len++;
				while (ptr_len[i])
				{
					if (ptr_len[i] == quot)
					{
						content = worddup(ptr_len, i);
						printf("%s\n", content);
						ptr_len += i;
						break ;
					}
					i++;
				}
			}
			else
				printf("Error de quotation!\n");
		}
		ptr_len++;
	}
}

// "main function"

int	has_double_quotation(char *str, char quot)
{
	int	ret;
	int	count;

	ret = 0;
	count = 0;
	printf("str = %s\n", str);
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
		return (NULL);
	offset = 0;
	while (offset < len)
	{
		str[offset] = s[offset];
		offset++;
	}
	str[offset] = '\0';
	return (str);
}

/* if (len)
		{
			res[counter] = worddup(s, len);
			if (res[counter++] == NULL)
				return (kill(res, counter - 1));
		}
 */


void	parse(char *line)
{
	int	i;

	i = 0;
	while ((size_t)i < strlen(line))
	{
		if (strchr(SEP, line[i]))
		{
			printf("oiii\n");
		}
		i++;
	}
}
