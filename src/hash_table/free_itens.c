/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_itens.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 12:17:26 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/23 21:50:24 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void free_item(t_hash_item *item)
{
	if (item->key != NULL)
	{
		free(item->key);
		free(item->value);
	}
	free(item);
}

void free_table(t_hash_table **table)
{
	int i;
	t_hash_item *current;
	t_hash_item *temp;

	i = 0;
	while (i < (*table)->size)
	{
		current = (*table)->item[i];
		while (current != NULL)
		{
			temp = current->next;
			free_item(temp);
			current = temp;
		}
		i++;
	}
	free((*table)->item);
	free((*table));
}