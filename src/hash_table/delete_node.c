/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete_node.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 12:19:00 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/23 21:44:49 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	delete_node(t_hash_table **table, t_hash_item *current, char *key);

void	table_delete(t_hash_table **table, char *key)
{
	t_hash_item	*current;
	int		index;

	index = hash_function(key);
	current = (*table)->item[index];
	if (current->next == NULL)
	{
		free_item(current);
		(*table)->item[index] = NULL;
		(*table)->count--;
	}
	else if (current->next != NULL)
		delete_node(table, current, key);
}

void	delete_node(t_hash_table **table, t_hash_item *current, char *key)
{
	t_hash_item	*temp;
	t_hash_item	*current_prev;
	int		index;

	temp = NULL;
	current_prev = current;
	while (current_prev && ft_strncmp(current_prev->key, key, ft_strlen(key) + 1))
	{
		temp = current_prev;
		current_prev = current_prev->next;
	}
	if (current_prev && temp)
	{
		temp->next = current_prev->next;
		free_item(current_prev);
	}
	else
	{
		index = hash_function(key);
		(*table)->item[index] = current->next;
		free_item(current);
	}
	(*table)->count--;
}
