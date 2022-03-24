/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 16:25:41 by lamorim           #+#    #+#             */
/*   Updated: 2022/03/24 16:10:10 by dmonteir         ###   ########.fr       */
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
	printf("%d\n", len);
	return (len);
}

void	ft_free_array(char ***mtx)
{
	int	i;

	i = 0;
	while ((*mtx)[i])
	{
		free((*mtx)[i]);
		i++;
	}
	free(*mtx);
}
