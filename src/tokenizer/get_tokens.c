/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_tokens.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 19:44:20 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/23 16:18:03 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

char	*char_cat(char *str, char c);

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
		if (*ptr != SPC && *ptr != QUOT && *ptr != DQUOT && *ptr != '|')
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
		else if (*ptr == '|')
		{
			tokens[i] = ft_strdup("|");
			i++;
			ptr++;
		}
	}
	tokens[i] = NULL;
	/* while (*tokens)
	{
		printf("%s\n", *tokens);
		tokens++;
	}
	printf("%s\n", *tokens); */
	return (tokens);
}

char	*worddup(char **s)
{
	char	*str;
	//size_t	offset;
	size_t	len;
	char	*temp;
	char	*join;

	if (*s == NULL)
		return (NULL);
	temp = *s;
	len = 0;
	str = NULL;
	join = ft_strdup("");

	while (!ft_strchr("| ", *temp) && *temp)
	{
		if (*temp == QUOT || *temp == DQUOT)
			temp++;
		else
		{
			str = join;
			join = char_cat(join, *temp);
			free(str);
			temp++;
		}
		len++;
	}

	/* while (!ft_strchr("| ", *str) && *str != '\0')
	{
		f("dentro do lex: %zu\n", len);
		len++;
		str++;
	} */
/* 	str = malloc(len + 1);
	offset = 0;
	while (offset < len)
	{
		str[offset] = (*s)[offset];
		offset++;
	}
	str[offset] = '\0'; */
	*s += len;
	return (join);
}

char	*char_cat(char *str, char c)
{
	char	*str_cat;
	size_t	len;
	int		i;

	i = 0;
	len = ft_strlen(str);
	str_cat = malloc(sizeof(char) * (len + 2));
	while(str[i])
	{
		str_cat[i] = str[i];
		i++;
	}
	str_cat[i] = c;
	str_cat[i + 1] = '\0';

	return (str_cat);
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
