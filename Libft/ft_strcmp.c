/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/14 21:31:11 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/02 18:55:47 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strcmp(char *a, char *b)
{
	int	index;

	index = 0;
	while (a[index] == b[index])
	{
		if (a[index] == '\0' || b[index] == '\0')
			break ;
		++index;
	}
	if (a[index] == '\0' && b[index] == '\0')
		return (1);
	else
		return (0);
}
