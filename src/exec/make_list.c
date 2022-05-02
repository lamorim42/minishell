/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_list.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/02 19:53:53 by lamorim           #+#    #+#             */
/*   Updated: 2022/05/02 20:07:04 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

t_pipe_list	*new_node(char *bin, char **args, char** envp)
{
	t_pipe_list	*list;

	list = (t_pipe_list *)malloc(sizeof(t_pipe_list));
	list->bin = bin;
	list->args = args;
	list->envp = envp;
	list->next = NULL;
	return (list);
}