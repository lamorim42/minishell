/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/13 18:35:38 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/20 16:16:36 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*worddup(char **s);
static int	make_red_len(char *str);
static int	make_quots_len(char *str);
static int	make_word_len(char *str);

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
	size_t	len;

	if (*s == NULL)
		return (NULL);
	len = 0;
	str = NULL;
	if (ft_strchr("|><", **s))
		len = make_red_len(*s);
	else if (ft_strchr("\"\'",  **s))
		len = make_quots_len(*s);
	else if (!ft_strchr("|><\"\' ", **s))
		len = make_word_len(*s);
	str = (char *)malloc(len + 1);
	if (str == NULL)
		return (NULL);
	ft_strlcpy(str, *s, len + 1);
	*s += len;
	return (str);
}

static int	make_red_len(char *str)
{
	char	red;
	int		len;

	red = *str;
	len = 1;
	if (red == '>' && str[len] == '>')
		len++;
	if (red == '<' && str[len] == '<')
		len++;
	return (len);
}

static int	make_quots_len(char *str)
{
	char	quot;
	int		len;

	quot = *str;
	len = 1;
	while (str[len] && str[len] != quot)
		len++;
	if (str[len] != '\0')
		len++;
	return (len);
}

static int	make_word_len(char *str)
{
	int	len;

	len = 1;
	while (str[len] && !ft_strchr("|><\"\' ", str[len]))
		len++;
	return (len);
}

