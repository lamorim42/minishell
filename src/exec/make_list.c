/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_list.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/02 19:53:53 by lamorim           #+#    #+#             */
/*   Updated: 2022/05/27 19:34:40 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_list(t_pipe_list *stack)
{
	t_pipe_list *temp;
	temp = stack;

	while(temp)
	{
		print_array("Array linked list", temp->args);
		temp = temp->next;
	}
	printf("----------------------\n");
}

void population_linked_list(t_line *line)
{
	t_pipe_list *temp;
	int i;

	temp = NULL;
	line->list_cmds = NULL;
	i = 0;
	while (line->cmds[i])
	{
		if (line->list_cmds == NULL)
		{
			line->list_cmds = new_node(line->cmds[i]);
		}
		else
		{
			temp = new_node(line->cmds[i]);
			add_back_list(&line->list_cmds, &temp);
		}
		i++;
	}
	//print_list(line->list_cmds);
}

void	add_back_list(t_pipe_list **list, t_pipe_list **node)
{
	t_pipe_list *temp;
	temp = *list;

	if (list == NULL)
	{
		*list = *node;
	}
	else
	{
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = (*node);
		(*node)->prev = temp;
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

void free_list(t_pipe_list *list)
{
	t_pipe_list *tmp;
	t_pipe_list *aux;

	tmp = list;
	while (tmp)
	{
		aux = tmp->next;
		if(tmp->args[0] && ft_strncmp(tmp->args[0], "PIPE", 4)
			&& ft_strncmp(tmp->args[0], "REDO", 4)
			&& ft_strncmp(tmp->args[0], "REDA", 4)
			&& ft_strncmp(tmp->args[0], "VAR", 4))
			free(tmp->bin);
		free(tmp);
		tmp = aux;
	}
}
