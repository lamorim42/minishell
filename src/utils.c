/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 09:38:59 by lamorim           #+#    #+#             */
/*   Updated: 2022/03/24 09:50:57 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_array(char *msg, char **array)
{
	int	i = 0;
	while (array[i])
	{
		printf("%s: %s\n", msg, array[i]);
		i++;
	}
	printf("%s\n", array[i]);
}