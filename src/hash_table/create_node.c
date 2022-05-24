/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_node.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 12:09:43 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/23 22:14:21 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_hash_item *create_item(char *key, char *value)
{
	t_hash_item *item;
	item = (t_hash_item *)malloc(sizeof(t_hash_item));

	item->key = ft_strdup(key);
	item->value = ft_strdup(value);

	return item;
}