/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/18 09:24:14 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/21 14:51:32 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

void	parse(char *line, t_stack **stacks)
{

	int	counter_red;
	size_t	i;
	t_node	*linked_list;
	i = 0;
	counter_red = 0;
	linked_list = split_line(line);
	while (i < strlen(line))
	{
		if (strchr(SEP, line[i]))
		{
			counter_red++;
			if (line[i + 1] == '>' || line[i + 1] == '<')
				i++;
		}
		i++;
	}
	cut_linked(stacks, linked_list, counter_red);
}

void	cut_linked(t_stack **stacks, t_node *linked_list, int counter_red)
{
	int	i;
	int	counter;

	counter = 0;
	i = 0;
	if (counter_red == 0)
	{
		(*stacks)->array_list = malloc(sizeof(t_node *));
		(*stacks)->array_list[0] = linked_list;
	}
	else
	{
		(*stacks)->array_list = malloc(sizeof(t_node *) * (counter_red + 1));
		while (linked_list != NULL)
		{
			printf("Antes do i++: %s\n", linked_list->str);
			(*stacks)->array_list[i] = copy_list(&linked_list);
			i++;
		}
	}
	while (counter < i)
	{
		print_list(&((*stacks)->array_list[counter]));
		counter++;
	}
}

t_node	*copy_list(t_node **linked_list)
{
	t_node *copy_array;

	copy_array = NULL;
	while ((*linked_list) != NULL
	&& !ft_strchr((*linked_list)->str, '|')
	&& !ft_strchr((*linked_list)->str, '>')
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
