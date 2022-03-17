/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 08:29:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/17 13:17:43 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	parse(char *line);
void	split_line(char *line);
char	*worddup(const char *s, size_t len);

int	main(void)
{
	char *line;

	line = readline("miau>");
	split_line(line);

	//parse(line);
	free(line);

	return (0);
}

void	split_line(char *line)
{
	//char	**splited;
	char	*ptr_len;
	size_t	i;
	char	*content;

	i = 0;
	ptr_len = line;
	while (*ptr_len)
	{
		if (*ptr_len == 39 || *ptr_len == 34)
		{
			//copiar tudo que tem dentro das aspas simples
			ptr_len++;
			while (ptr_len[i])
			{
				//printf("%c\n", ptr_len[i]);
				if (ptr_len[i] == 39 || ptr_len[i] == 34)
				{
					content = worddup(ptr_len, i);
					printf("%s\n", content);
					break ;
				}
				i++;
			}
			//printf("content: %s\n", content);
		}
		/* else if (line[i] == ' ')
		{

		} */
		ptr_len++;
		//printf("%s\n",ptr_len);
	}
	//return (void);
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
