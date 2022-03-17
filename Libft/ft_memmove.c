/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/24 10:34:33 by dmonteir          #+#    #+#             */
/*   Updated: 2021/05/28 15:44:48 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *s1, const void *s2, size_t n)
{
	size_t			i;
	unsigned char	*str1;
	unsigned char	*str2;

	i = 0;
	str1 = (unsigned char *)s1;
	str2 = (unsigned char *)s2;
	if (str1 == str2)
		return (str1);
	if ((str2 < str1) && ((str2 + n) > str1))
		while (i++ < n)
			str1[n - i] = str2[n - i];
	else if ((str2 > str1) && ((str1 + n) > str2))
	{
		while (i < n)
		{
			str1[i] = str2[i];
			i++;
		}
	}
	else
		ft_memcpy(s1, s2, n);
	return (str1);
}
