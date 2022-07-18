/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:04:40 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/18 14:57:27 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_path_ok(t_hash_table **table);
static int	make_redirections(t_pipe_list *node);

void	exec_path(t_line *line, t_pipe_list *list, t_hash_table **table)
{
	int	input;

	if (is_path_ok(table))
	{
		input = make_redirections(list);
		if (input < 0)
			free_exit(line, *table, 1);
		if (!list->bin)
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
	free_exit(line, *table, 0);
}

static int	is_path_ok(t_hash_table **table)
{
	char	*path;
	int		vrf;

	vrf = 1;
	path = search_item(*table, "PATH");
	if (path == NULL)
		vrf = 0;
	free(path);
	return (vrf);
}

static int	make_redirections(t_pipe_list *node)
{
	int	input;

	input = find_input(node);
	find_output(node);
	close_fds(node);
	return (input);
}
