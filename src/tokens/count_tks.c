/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_tks.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:37:57 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/12 18:00:05 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//# include "minishell.h"
# include "../Libft/libft.h"
int	count_tks(char *line);


int	count_tks(char *line)
{
	int	counter;
	int	flag;

	flag = 0;
	counter = 0;
	while (*line)
	{
		if (ft_strchr("|", *line))
			counter++;
		if (!flag && !ft_strchr("| ", *line))
		{
			flag = 1;
			counter++;
		}
		else if (flag && ft_strchr("| ", *line))
			flag = 0;
		line++;
	}
	return (counter);
}