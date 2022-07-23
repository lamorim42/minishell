/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:04:40 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 17:13:16 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	make_redirections(t_pipe_list *node);

void	exec_path(t_line *line, t_pipe_list *list, t_hash_table **table)
{
	int	input;

	input = make_redirections(list);
	if (input < 0)
		free_exit(line, *table, 1);
	if (list->bin == NULL)
	{
		error_msg(list->args[0], ": command not found\n");
		free_exit(line, *table, 127);
	}
	else if (ft_strcmp_len(list->bin, "builtin"))
	{
		exec_builtins(list, table);
		free_exit(line, *table, 0);
	}
	else if (execve(list->bin, list->args, line->envp) == -1)
	{
		perror(list->args[0]);
		free_exit(line, *table, 127);
	}
}

static int	make_redirections(t_pipe_list *node)
{
	int	input;

	input = find_input(node);
	find_output(node);
	close_fds(node);
	return (input);
}
