/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_list.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/02 19:53:53 by lamorim           #+#    #+#             */
/*   Updated: 2022/07/06 19:29:13 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	add_back_list(t_pipe_list **list, t_pipe_list *node)
{
	t_pipe_list	*temp;

	temp = *list;
	if (*list == NULL)
		*list = node;
	else
	{
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = node;
		node->prev = temp;
	}
}

t_pipe_list	*new_node(char **args)
{
	t_pipe_list	*node;

	node = (t_pipe_list *)malloc(sizeof(t_pipe_list));
	node->prev = NULL;
	node->next = NULL;
	node->bin = NULL;
	node->args = args;
	return (node);
}
