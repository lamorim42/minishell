/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/13 18:35:38 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/18 10:39:39 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
char	*worddup(char **s);

char	**tokenizer(t_line *line)
{
	char	**tokens;
	int		count;
	int		i = 0;
	char	*ptr;

	count = count_tks(line->str);
	tokens = (char **)malloc(sizeof(char *) * (count + 1));
	if (!tokens)
		return (NULL);
	ptr = line->str;
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
	//print_array("Tokens", tokens);
	return (tokens);
}

char	*worddup(char **s)
{
	char	*str;
	size_t	len;

	if (*s == NULL)
		return (NULL);
	len = 0;
	str = NULL;
	if (ft_strchr("|>", **s))
		len++;
	else
	{
		while ((*s)[len] && !ft_strchr("|> ", (*s)[len]))
			len++;
	}
	str = (char *)malloc(len + 1);
	if (str == NULL)
		return(NULL);
	ft_strlcpy(str, *s, len + 1);
	*s += len;
	return (str);
}
