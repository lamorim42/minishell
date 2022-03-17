/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/27 11:39:59 by dmonteir          #+#    #+#             */
/*   Updated: 2021/06/10 12:23:33 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *str1)
{
	int		i;
	int		len;
	char	*new_str;

	len = 0;
	while (str1[len])
		len++;
	new_str = (char *)malloc(sizeof(char) * (len + 1));
	if (!(new_str))
		return (NULL);
	i = 0;
	while (str1[i])
	{
		new_str[i] = str1[i];
		i++;
	}
	new_str[i] = '\0';
	return (new_str);
}
