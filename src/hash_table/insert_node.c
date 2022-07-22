/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   insert_node.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 12:19:22 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/22 13:44:44 by lamorim          ###   ########.fr       */
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
		if (crt->key && (ft_strcmp_len(crt->key, key)))
		{
			free (crt->value);
			crt->value = ft_strdup(value);
			free_item(item);
		}
		else
			handle_collision(table, item, index);
	}
}

void	handle_collision(t_hash_table **table, t_hash_item *item, int index)
{
	t_hash_item	*current;

	current = (*table)->item[index];
	while (current->next != NULL)
		current = current->next;
	current->next = item;
	current->next->next = NULL;
	(*table)->count++;
}
