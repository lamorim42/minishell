/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_tokens.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 19:44:20 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/22 15:57:15 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

char	**token(char *line)
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
	while (*ptr)
	{
		if (*ptr != SPC && *ptr != QUOT && *ptr != DQUOT)
		{
			tokens[i] = worddup(&ptr);
			i++;
		}
		else if (*ptr == SPC)
			ptr++;
		else if (*ptr == QUOT || *ptr == DQUOT)
		{
			tokens[i] = quotdup(&ptr);
			i++;
		}
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
	str = *s;
	len = 0;
	while (*str != SPC && *str != '\0')
	{
		len++;
		str++;
	}
	str = malloc(len + 1);
	offset = 0;
	while (offset < len)
	{
		str[offset] = (*s)[offset];
		offset++;
	}
	str[offset] = '\0';
	*s += len;
	return (str);
}

char	*quotdup(char **s)
{
	char	*str;
	size_t	offset;
	size_t	len;

	if (*s == NULL)
		return (NULL);
	str = *s + 1;
	len = 0;
	if (has_double_quotation(str, **s))
		return_error();
	while (*str != **s)
	{
		len++;
		str++;
	}
	str = malloc(len + 1);
	offset = 0;
	while (offset < len)
	{
		str[offset] = (*s + 1)[offset];
		offset++;
	}
	str[offset] = '\0';
	*s += len + 2;
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
