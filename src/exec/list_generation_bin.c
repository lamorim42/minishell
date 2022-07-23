/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list_commands.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 19:19:07 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/21 16:2 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_path(char *str);

void	list_generation_bin(t_line *line, t_hash_table **table)
{
	t_pipe_list	*temp;

	temp = line->list_cmds;
	while (temp)
	{
		if (is_a_builtin(&temp->args[0]) == 0)
			temp->bin = ft_strdup("builtin");
		else if (temp->args && temp->args[0]
			&& is_command(temp))
		{
			if (is_path(temp->args[0]))
				temp->bin = ft_strdup(temp->args[0]);
			else
				temp->bin = path_finder(temp->args[0], table);
		}
		temp = temp->next;
	}
}

static int	is_path(char *str)
{
	return (!ft_strncmp(str, "/", 1)
		|| !ft_strncmp(str, "./", 2)
		|| !ft_strncmp(str, "../", 3));
}
