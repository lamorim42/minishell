/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 08:29:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/21 16:41:05 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


int	main(void)
{
	char *line;

	line = readline("miau> ");

	free(line);

	return (0);
}

char	**token(const char *line)
{
	char	**tokens;
	size_t	words;
	char	*ptr;
	int		i;

	if (!line)
		return (NULL);
	words = count_words(line);
	tokens = malloc(sizeof(char *) * (words + 1));
	i = 0;
	ptr = line;
	while (ptr)
	{
		if (*ptr != SPACE)
		{
			tokens[i] = worddup(&ptr);
			i++;
		}
		else if (*ptr == SPACE)
			ptr++;
		else if (*ptr == QUOT || *ptr == DQUOT)
	}

}

size_t	count_words(const char *s)
{
	int		is_word;
	size_t	words;

	words = 0;
	is_word = 0;
	while (*s)
	{
		if (!is_word && *s != SPACE)
		{
			is_word = 1;
			words++;
		}
		else if (is_word && *s == SPACE)
			is_word = 0;
		s++;
	}
	return (words);
}

char	*worddup(const char **s)
{
	char	*str;
	size_t	offset;
	size_t	len;

	if (*s == NULL)
		return (NULL);
	str = *s;
	len = 0;
	while (*str != SPACE && *str != NULL)
	{
		len++;
		str++;
	}
	str = malloc(len + 1);
	offset = 0;
	while (offset < len)
	{
		str[offset] = s[offset];
		offset++;
	}
	str[offset] = '\0';
	*s += len;
	return (str);
}

char	*quotdup(const char **s)
{
	char	*str;
	size_t	offset;
	size_t	len;

	if (*s == NULL)
		return (NULL);
	str = *(s + 1);
	len = 0;
	if (!has_double_quotation(str, *s))
		return (NULL);
	while (*str != SPACE && *str != NULL)
	{
		len++;
		str++;
	}
	str = malloc(len + 1);
	offset = 0;
	while (offset < len)
	{
		str[offset] = s[offset];
		offset++;
	}
	str[offset] = '\0';
	*s += len;
	return (str);
}

int	has_double_quotation(char *str, char quot)
{
	int	ret;
	int	count;

	ret = 0;
	count = 0;
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