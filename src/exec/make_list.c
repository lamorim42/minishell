/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_list.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/02 19:53:53 by lamorim           #+#    #+#             */
/*   Updated: 2022/05/10 20:22:10 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_list(t_pipe_list *stack);

// t_node    *new_node(char *str)
// {
//     t_node    *node;

//     node = malloc(sizeof(t_node));
//     if (!node)
//         return (NULL);
//     else
//     {
//         node->str = str;
//         node->next = NULL;
//     }
//     return (node);
// }

// ls -la | echo oi
// bin = /usr/lin/ls
// args = "ls", "-la"
// envp = envp

/*
t_node	*copy_list(t_node **linked_list)
{
	t_node *copy_array;

	copy_array = NULL;
	while ((*linked_list) != NULL \
	&& !ft_strchr((*linked_list)->str, '|') \
	&& !ft_strchr((*linked_list)->str, '>') \
	&& !ft_strchr((*linked_list)->str, '<'))
	{
		if (copy_array == NULL)
			copy_array = new_node((*linked_list)->str);
		else
			add_back_stack((*linked_list)->str, &copy_array);
		(*linked_list) = (*linked_list)->next;
	}
	if ((*linked_list) != NULL)
	{
		add_back_stack((*linked_list)->str, &copy_array);
		(*linked_list) = (*linked_list)->next;
	}
	return (copy_array);
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
}  */

/* void add_back_stack(t_pipe_list *list)
{

} */

//ls -a | echo oi

void	print_list(t_pipe_list *stack)
{
	t_pipe_list *temp;
	temp = stack;

	while(temp)
	{
		print_array("Array linked list", temp->args);
		printf("%s\n", temp->bin);
		temp = temp->next;
	}
	printf("----------------------\n");
}

void population_linked_list(t_line *line)
{
	t_pipe_list *temp;
	int i;
	i = 0;

	while (line->cmds[i])
	{
		if (line->list_cmds == NULL)
		{
			line->list_cmds = new_node(line->bin, line->cmds[i], line->envp);
		}
		else
		{
			temp = new_node(line->bin, line->cmds[i], line->envp);
			add_back_list(&line->list_cmds, &temp);
		}
		i++;
	}

	print_list(line->list_cmds);
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
		temp->next = *node;
	}
}

t_pipe_list	*new_node(char *bin, char **args, char** envp)
{
	t_pipe_list	*node;

	node = (t_pipe_list *)malloc(sizeof(t_pipe_list));
	node->bin = bin;
	node->args = args;
	node->envp = envp;
	node->next = NULL;
	return (node);
}
