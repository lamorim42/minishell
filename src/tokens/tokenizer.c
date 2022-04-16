/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/13 18:35:38 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/16 19:36:47 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
char	*worddup(char **s);

char	**tokenizer(char *line)
{
	char	**tokens;
	int		count;
	int		i = 0;
	char	*ptr;
	count = count_tks(line);

	tokens = malloc(sizeof(char *) * (count + 1));
	ptr = line;
	while(*ptr)
	{
		if (*ptr && *ptr != ' ')
		{
			tokens[i] = worddup(&ptr);
			i++;
		}
		else
			ptr++;
	}
	tokens[i] = NULL;
	return (tokens);
}

char	*worddup(char **s)
{
	char	*str;
	size_t	offset;
	size_t	len;

	if (*s == NULL)
		return (NULL);
	len = 0;
	str = NULL;

	while ((*s)[len] && !ft_strchr("| ", (*s)[len]))
		len++;
	str = malloc(len + 1);
	if (str == NULL)
		return(NULL);
	offset = 0;
	ft_strlcpy(str, *s, len + 1);
	*s += len;
	return (str);
}