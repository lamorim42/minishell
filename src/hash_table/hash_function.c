/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash_function.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 18:26:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/21 19:54:32 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

unsigned long hash_function(char *str)
{
	unsigned long i;
	int j;

	j = 0;
	i = 0;
	while (str[j])
	{
		i += str[j];
		j++;
	}
	return i % size_table;
}
