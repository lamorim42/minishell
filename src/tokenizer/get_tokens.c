/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_tokens.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 19:44:20 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/21 20:25:50 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

char	**token(char *line)
{
	char	**tokens;
	size_t	words;
	char	*ptr;
	int		i;
	//int		counter = 0;

	if (!line)
		return (NULL);
	words = count_words(line);
	tokens = malloc(sizeof(char *) * (words + 1));
	i = 0;
	ptr = line;
	while (*ptr)
	{
		//printf("while do ptr\n");
		if (*ptr != SPC && *ptr != QUOT && *ptr != DQUOT)
		{
			//printf("i do space = %d\n", i);
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
//	while (counter < i)
//	{
//		printf("Tokens: %s\n", tokens[counter]);
//		counter++;
//	}
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
		//printf("while1 do worddup\n");
		len++;
		str++;
	}
	//printf("%zu\n", len);
	str = malloc(len + 1);
	offset = 0;
	while (offset < len)
	{
		//printf("while do worddup\n");
		str[offset] = (*s)[offset];
		offset++;
	}
	str[offset] = '\0';
	*s += len;
	return (str);
}

// miau "gatinho" "oi"

char	*quotdup(char **s)
{
	char	*str;
	size_t	offset;
	size_t	len;

	if (*s == NULL)
		return (NULL);
	str = *s + 1;
	//printf("%s\n", str);
	len = 0;
	if (has_double_quotation(str, **s))
		return_error();
	while (*str != **s)
	{
		//printf("while do quotdup\n");
		len++;
		str++;
	}
	str = malloc(len + 1);
	offset = 0;
	while (offset < len)
	{
		//printf("while2 do quotdup\n");
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
