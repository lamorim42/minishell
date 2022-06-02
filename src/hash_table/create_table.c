/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_table.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 12:09:39 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/02 18:59:10 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_hash_table	*create_table(int size)
{
	t_hash_table	*table;
	int				i;

	i = 0;
	table = (t_hash_table *)malloc(sizeof(t_hash_table));
	table->size = size;
	table->count = 0;
	table->item = (t_hash_item **)ft_calloc(sizeof(t_hash_item *), size);
	while (i < table->size)
	{
		table->item[i] = NULL;
		i++;
	}
	return (table);
}
