/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/13 18:35:38 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/16 10:38:00 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char		*worddup(char **s);
static int	make_a_word_by_str(char *s, char c);

char	**tokenizer(t_line *line)
{
	char	**tokens;
	int		count;
	int		i;
	char	*ptr;

	i = 0;
	count = count_tks(line->str);
	tokens = (char **)malloc(sizeof(char *) * (count + 1));
	if (!tokens)
		return (NULL);
	ptr = line->str;
	while (*ptr)
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
	size_t	len;

	if (*s == NULL)
		return (NULL);
	len = 0;
	str = NULL;
	if (ft_strchr("|><", **s))
	{
		len++;
		if (ft_strchr(">", **s) && ft_strchr(">", *((*s) + 1)))
			len++;
		if (ft_strchr("<", **s) && ft_strchr("<", *((*s) + 1)))
			len++;
	}
	else if (**s != '\"' && **s != '\'')
		while ((*s)[len] && !ft_strchr("|>< ", (*s)[len]))
			len++;
	else
	{
		len = make_a_word_by_str(*s, **s);
	}
	str = (char *)malloc(len + 1);
	if (str == NULL)
		return (NULL);
	ft_strlcpy(str, *s, len + 1);
	*s += len;
	return (str);
}

static int	make_a_word_by_str(char *s, char c)
{
	int	len;

	len = 0;
	while (s[len] && s[len] == c)
	{
		len++;
	}

	while (s[len] && s[len] != c)
	{
		len++;
	}

	while (s[len] && s[len] == c)
	{
		len++;
	}

	return len;
}
