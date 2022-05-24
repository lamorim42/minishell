/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_table.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 12:09:39 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/23 21:00:25 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_hash_table *create_table(int size)
{
	t_hash_table *table;
	int i;

	i = 0;
	table = (t_hash_table *)malloc(sizeof(t_hash_table));
	table->size = size;
	table->count = 0;
	table->item = (t_hash_item **)ft_calloc(sizeof(t_hash_item *), size);

	while (i < table->size)
	{
		//table->item[i] = NULL;
		table->item[i] = (t_hash_item *)ft_calloc(sizeof(t_hash_item), size);
		//table->item[i]->key = NULL;
		//table->item[i]->value = NULL;
		i++;
	}

	return table;
}