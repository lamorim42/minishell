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


void	list_generation_bin(t_line *line)
{
	t_pipe_list	*temp;

	temp = line->list_cmds;
	while (temp)
	{
		if (is_a_builtin(&temp->args[0]) == 0)
			temp->bin = ft_strdup("builtin");
		else if (temp->args && temp->args[0]
			&& ft_strncmp(temp->args[0], "PIPE", 4)
			&& ft_strncmp(temp->args[0], "REDO", 4)
			&& ft_strncmp(temp->args[0], "REDA", 4)
			&& ft_strncmp(temp->args[0], "REDI", 4))
			temp->bin = path_finder(line, temp->args[0]);
		temp = temp->next;
	}
}

int	is_a_builtin(char **node)
{
	if (ft_strncmp(*node, "echo", 4) == 0
		|| ft_strncmp(*node, "cd", 2) == 0
		|| ft_strncmp(*node, "pwd", 3) == 0
		|| ft_strncmp(*node, "export", 6) == 0
		|| ft_strncmp(*node, "unset", 5) == 0
		|| ft_strncmp(*node, "env", 3) == 0
		|| ft_strncmp(*node, "exit", 4) == 0)
		return (0);
	return (1);
}

void	exec_list(t_line *line,  t_hash_table **table)
{
	t_pipe_list	*temp;

	temp = line->list_cmds;
	while (temp)
	{
		if (!temp->prev && (!ft_strncmp(temp->args[0], "REDO", 4)
				|| !ft_strncmp(temp->args[0], "REDA", 4)))
		{
			temp->fd[0] = open(temp->args[1], O_WRONLY | O_CREAT
					| (O_TRUNC * (ft_strncmp(temp->args[0], "REDO", 4) == 0))
					| (O_APPEND * ft_strncmp(temp->args[0], "REDA", 4) == 0),
					0644);
			if (temp->fd[0] == -1)
			{
				perror(temp->args[1]);
				free_line(line);
				free_table(table);
			}
			close(temp->fd[0]);
		}
		if (ft_strncmp(temp->args[0], "exit", 4) == 0)
			exit_builtin(line, temp, table);
		init_fork(line, temp, table);
		temp = temp->next;
	}
}

void	exec_builtins(t_pipe_list *node, t_hash_table **table)
{
	if (ft_strncmp(node->args[0], "echo", 4) == 0)
		echo_builtin(node);
	else if (ft_strncmp(node->args[0], "cd", 2) == 0)
		cd_builtin(node, table);
	else if (ft_strncmp(node->args[0], "pwd", 3) == 0)
		pwd_builtin(node, table);
	else if (ft_strncmp(node->args[0], "export", 6) == 0)
		export_builtin(node, table);
	else if (ft_strncmp(node->args[0], "unset", 5) == 0)
		unset_builtin(node, table);
	else if (ft_strncmp(node->args[0], "env", 3) == 0)
		env_builtin(node, table);
}
