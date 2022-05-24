/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   linked_list.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/19 19:39:27 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/19 20:31:02 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_node *init_node_list()
{
	t_node *list;

	list = malloc(sizeof(t_node));
	return (list);
}

t_node *add_list(t_node *list, t_hash_item *item)
{
	if (list == NULL)
	{
		list = init_node_list();
		list->item = item;
		list->next = NULL;
		return list;
	}
	else if (list->next == NULL)
	{
		list = init_node_list();
		list->item = item;
		list->next = NULL;
		return list;
	}
	return (list);
}
