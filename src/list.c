/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 09:46:03 by lamorim           #+#    #+#             */
/*   Updated: 2022/03/19 19:58:04 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_node	*new_node(char *str)
{
	t_node	*node;

	node = malloc(sizeof(t_node));
	if (!node)
		return (NULL);
	else
	{
		node->str = str;
		node->next = NULL;
	}
	return (node);
}

void	add_back_stack(char *content, t_node **head)
{
	t_node *node;
	t_node *temp;

	if (!*head)
		return ;
	temp = (*head);
	node = new_node(content);
	while (temp->next)
		temp = temp->next;
	temp->next = node;
	//printf("add new node\n");
}

/* void create_stack(t_stack **stack)
{
	t_node *temp;
	temp = (*stack)->
} */

void	print_list(t_node **stack)
{
	t_node *temp;
	temp = (*stack);

	while(temp)
	{
		printf("%s\n", temp->str);
		temp = temp->next;
	}
	printf("----------------------\n");
}

void	clean_list(t_node **head)
{
	t_node	*temp;
	t_node	*aux;

	if (!*head)
		return ;
	temp = *head;
	while (temp)
	{
		aux = temp->next;
		free(temp->str);
		free(temp);
		temp = aux;
	}
}