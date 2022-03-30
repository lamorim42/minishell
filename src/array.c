/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 16:25:41 by lamorim           #+#    #+#             */
/*   Updated: 2022/03/30 02:11:24 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


//Tá errada
int	ft_array_len(char **array)
{
	int	len;
	char	**temp;

	temp = array;
	len = 0;
	while (*temp)
	{
		len++;
		temp++;
	}
	return (len);
}

void	ft_free_array(char **mtx)
{
	int	i;

	i = 0;
	while ((mtx)[i])
	{
		free((mtx)[i]);
		i++;
	}
	free(mtx);
}
