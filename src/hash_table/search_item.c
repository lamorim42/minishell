/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search_item.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 15:43:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/03 10:46:53 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*search_item(t_hash_table *table, char *key)
{
	t_hash_item	*current;
	int			index;

	index = hash_function(key);
	current = table->item[index];
	while (current != NULL)
	{
		if (current->key && ft_strncmp(current->key, key,
				ft_strlen(key) + 1) == 0)
		{
			return (ft_strdup(current->value));
		}
		current = current->next;
	}
	return (NULL);
}
