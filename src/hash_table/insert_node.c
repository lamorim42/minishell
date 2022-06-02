/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   insert_node.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 12:19:22 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/02 18:40:50 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_collision(t_hash_table **table, t_hash_item *item, int index);

void	hash_insert(t_hash_table **table, char *key, char *value)
{
	int			index;
	t_hash_item	*item;
	t_hash_item	*crt;

	index = hash_function(key);
	item = create_item(key, value);
	crt = (*table)->item[index];
	if (crt == NULL)
	{
		(*table)->item[index] = item;
		(*table)->count++;
	}
	else
	{
		if (*(crt->key) && (ft_strncmp(crt->key, key, ft_strlen(key) + 1) == 0))
		{
			free (crt->value);
			crt->value = ft_strdup(value);
			free_item(item);
		}
		else
		{
			handle_collision(table, item, index);
		}
	}
}

void	handle_collision(t_hash_table **table, t_hash_item *item, int index)
{
	t_hash_item	*current;

	current = (*table)->item[index];
	while (current->next != NULL)
	{
		current = current->next;
	}
	current->next = item;
	current->next->next = NULL;
	(*table)->count++;
}
