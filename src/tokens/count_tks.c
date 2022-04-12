/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_tks.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:37:57 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/11 20:47:00 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//# include "minishell.h"

int	count_tks(char *line)
{
	int counter;

	counter = 0;
	while (*line)
	{
		if (*line == ' ')
			counter++;
		line++;
	}
	if (*line == '\0')
		counter++;
	return (counter);
}