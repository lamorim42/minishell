/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_tks.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:37:57 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 13:36:49 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//# include "minishell.h"
#include "../../Libft/libft.h"
#include <stdio.h>

static void	count_special_chr(char **line, int *count);
static void	count_inside_quot_as_one_word(char **line, int *count);
static void	count_word(char **line, int *count);

int	count_tks(char *line)
{
	int		counter;

	counter = 0;
	while (*line)
	{
		if (ft_strchr("|><", *line))
			count_special_chr(&line, &counter);
		else if (ft_strchr("\"\'", *line))
			count_inside_quot_as_one_word(&line, &counter);
		else if (!ft_strchr("|><\"\' ", *line))
			count_word(&line, &counter);
		else if (ft_strchr(" ", *line))
			line++;
	}
	return (counter);
}

static void	count_special_chr(char **line, int *count)
{
	char	red;

	red = **line;
	(*count)++;
	(*line)++;
	if (**line != '\0' && ft_strchr("\"\'", **line))
		return (count_inside_quot_as_one_word(line, count));
	if (red == '>' && **line == '>')
		(*line)++;
	else if (red == '<' && **line == '<')
		(*line)++;
}

static void	count_inside_quot_as_one_word(char **line, int *count)
{
	char	quot;

	(*count)++;
	quot = **line;
	(*line)++;
	while (*(*line + 1) && **line != quot)
		(*line)++;
	(*line)++;
	while (**line && !ft_strchr("|><$ ", **line))
	{
		if (ft_strchr("\"\'", **line))
		{
			(*count)--;
			return (count_inside_quot_as_one_word(line, count));
		}
		(*line)++;
	}
}

static void	count_word(char **line, int *count)
{
	char	var;

	var = **line;
	(*count)++;
	while (**line && !ft_strchr("|>< ", **line))
	{
		if (ft_strchr("\"\'", **line) && var != '$')
		{
			(*count)--;
			return (count_inside_quot_as_one_word(line, count));
		}
		else if (ft_strchr("\"\'", **line) && var == '$')
			return ;
		(*line)++;
	}
}
