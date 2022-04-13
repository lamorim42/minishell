/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_tks.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:37:57 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/12 20:51:26 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//# include "minishell.h"
#include "../Libft/libft.h"

static void	update_counter_flag(int *counter, int *flag);
static void	count_inside_quot_as_one_word(char **line);

int	count_tks(char *line)
{
	int		counter;
	int		flag;

	flag = 0;
	counter = 0;
	while (*line)
	{
		if (ft_strchr("|", *line))
			counter++;
		if (!flag && ft_strchr("\"\'", *line))
		{
			count_inside_quot_as_one_word(&line);
			update_counter_flag(&counter, &flag);
		}
		else if (!flag && !ft_strchr("| ", *line))
			update_counter_flag(&counter, &flag);
		else if (flag && ft_strchr("| ", *line))
			flag = 0;
		line++;
	}
	return (counter);
}

static void	update_counter_flag(int *counter, int *flag)
{
	(*counter)++;
	*flag = 1;
}

static void	count_inside_quot_as_one_word(char **line)
{
	char	quot;

	quot = **line;
	(*line)++;
	while (*(*line + 1) && **line != quot)
		(*line)++;
}
