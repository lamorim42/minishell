/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_hash.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/14 19:49:43 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 14:38:32 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_table(t_hash_table **table)
{
	t_hash_item	*current;
	t_hash_item	*temp;
	int			i;
	int			counter;

	current = NULL;
	i = 0;
	counter = 0;
	while (i < (*table)->size)
	{
		if ((*table)->item[i])
		{
			current = (*table)->item[i];
			while (current != NULL)
			{
				temp = current->next;
				if (current->key != NULL)
					printf("%s=%s\n", current->key, current->value);
				current = temp;
				counter++;
			}
			counter++;
		}
		i++;
	}
}
