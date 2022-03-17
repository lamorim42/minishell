/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/26 21:29:01 by dmonteir          #+#    #+#             */
/*   Updated: 2021/05/28 15:55:55 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nitems, size_t size)
{
	char	*ptr;
	size_t	i;

	i = 0;
	ptr = malloc(nitems * size);
	if (ptr == 0)
		return (NULL);
	else
	{
		while (i < (nitems * size))
		{
			ptr[i] = '\0';
			i++;
		}
		return (ptr);
	}
}
