/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/13 18:35:38 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/21 17:27:40 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*worddup(char **s);
static void	make_red_len(char *str, int *len);
static void	make_quots_len(char *str, int *len);
static void	make_word_len(char *str, int *len);

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

static char	*worddup(char **s)
{
	char	*str;
	int	len;

	if (*s == NULL)
		return (NULL);
	len = 0;
	str = NULL;
	if (ft_strchr("|><", **s))
		make_red_len(*s, &len);
	else if (ft_strchr("\"\'",  **s))
		make_quots_len(*s, &len);
	else if (!ft_strchr("|><\"\' ", **s))
		make_word_len(*s, &len);
	str = (char *)malloc(len + 1);
	if (str == NULL)
		return (NULL);
	ft_strlcpy(str, *s, len + 1);
	*s += len;
	return (str);
}

static void	make_red_len(char *str, int *len)
{
	char	red;

	red = *str;
	(*len)++;
	if (red == '>' && str[*len] == '>')
		(*len)++;
	if (red == '<' && str[*len] == '<')
		(*len)++;
}

static void	make_quots_len(char *str, int *len)
{
	char	quot;

	quot = *str;
	(*len)++;
	while (str[*len] && str[*len] != quot)
		(*len)++;
	if (str[*len] == quot)
		(*len)++;
	if (quot == '\'')
		return ;
	while (str[*len] && !ft_strchr("|>< ", str[*len]))
	{
		if (str[*len] == '\"')
		{
			return (make_quots_len(str, len));
		}
		(*len)++;
	}
}

static void	make_word_len(char *str, int *len)
{
	(*len)++;
	while (str[*len] && !ft_strchr("|><\' ", str[*len]))
	{
		if (str[*len] == '\"')
			return (make_quots_len(str, len));
		(*len)++;
	}
}

