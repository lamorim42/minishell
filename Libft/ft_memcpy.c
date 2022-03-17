/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/22 12:59:19 by dmonteir          #+#    #+#             */
/*   Updated: 2021/05/29 01:05:20 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*s1;
	unsigned char	*s2;

	i = 0;
	s1 = (unsigned char *)dest;
	s2 = (unsigned char *)src;
	if (!s1 && !s2)
		return (0);
	if (s1 == s2)
		return (s1);
	while (i < n)
	{
		s1[i] = s2[i];
		i++;
	}
	return (dest);
}
