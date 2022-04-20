/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_arrcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/19 20:44:11 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/19 21:02:35 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_arrcmp(const char **arr1, const char **arr2)
{
	int i = 0;

	if (arr1 == NULL || arr2 == NULL)
		return (NULL);
	while (arr1[i] && arr2[i])
	{
		if (ft_strncmp(arr1[i], arr2[i], ft_strlen(arr1[i])))
			return (1);
		i++;
	}
	return (0);
}