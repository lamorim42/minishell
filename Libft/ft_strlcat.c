/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 15:38:13 by dmonteir          #+#    #+#             */
/*   Updated: 2021/05/27 16:24:03 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t	dest_i;
	size_t	src_i;
	size_t	i;
	size_t	j;

	dest_i = ft_strlen(dest);
	src_i = ft_strlen(src);
	i = dest_i;
	j = 0;
	if (dest_i >= size)
		return (src_i + size);
	while (src[j] != '\0' && i < (size - 1))
	{
		dest[i] = src[j];
		i++;
		j++;
	}
	dest[i] = '\0';
	return (src_i + dest_i);
}
