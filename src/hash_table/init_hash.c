/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_hash.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/14 19:49:43 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/17 13:41:16 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_hash(void)
{
	t_hash_table	*table;

	table = create_table(SIZE_TABLE);
	hash_insert(&table, "usr/bin/ls", "ls");
	hash_insert(&table, "24", "oi222");
	print_search(&table, "usr/bin/ls");
	print_search(&table, "24");
	print_search(&table, "3");
	table_delete(&table, "usr/bin/ls");
	print_search(&table, "usr/bin/ls");
	free_table(&table);
}

char	*print_search(t_hash_table **table, char *key)
{
	char	*val;

	val = search_item(table, key);
	if (val == NULL)
	{
		printf("Key:%s does not exist\n", key);
		return NULL;
	}
	else
		printf("Key:%s, Value:%s\n", key, val);
	return (val);
}

void	print_table(t_hash_table **table)
{
	t_hash_item	*current;
	t_hash_item	*temp;
	int			i;

	printf("\nHash Table\n-------------------\n");
	i = 0;
	while (i < (*table)->size)
	{
		if ((*table)->item[i])
		{
			temp = current->next;
			printf("Index:%d, Key:%s, Value:%s\n", i, current->key,
				current->value);
			current = temp;
		}
		i++;
	}
	printf("-------------------\n\n");
}
