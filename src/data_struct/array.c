/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 16:25:41 by lamorim           #+#    #+#             */
/*   Updated: 2022/03/31 21:54:39 by lamorim          ###   ########.fr       */
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

char	**copy_array(char **tokens, int size)
{
	char	**array_cpy;
	int		len;
	int		i;


	len = ft_array_len(tokens);
	array_cpy = malloc(sizeof(char *) * (len + 1));

	i = 0;
	while (i < size)
	{
		array_cpy[i] = ft_strdup(tokens[i]);
		i++;
	}
	array_cpy[i] = NULL;
	return(array_cpy);
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
