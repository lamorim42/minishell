/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_arrcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/19 20:44:11 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/02 18:56:21 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_arrcmp(char **arr1, char **arr2)
{
	int	i;

	i = 0;
	if (arr1 == NULL || arr2 == NULL)
		return (-1);
	while (arr1[i] && arr2[i])
	{
		if (ft_strncmp(arr1[i], arr2[i], ft_strlen(arr1[i])))
			return (1);
		i++;
	}
	if (arr1[i] || arr2[i])
		return (1);
	return (0);
}
