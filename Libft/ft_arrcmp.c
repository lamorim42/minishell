/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_arrcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/19 20:44:11 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/19 21:40:19 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

int	ft_arrcmp(char **arr1,char **arr2)
{
	int i = 0;

	if (arr1 == NULL || arr2 == NULL)
		return (-1);
	while (arr1[i] && arr2[i])
	{
		printf("Estamos comparando pela %d vez\n", i);
		if (ft_strncmp(arr1[i], arr2[i], ft_strlen(arr1[i])))
			return (1);
		i++;
	}
	if (arr1[i] || arr2[i])
		return (1);
	return (0);
}