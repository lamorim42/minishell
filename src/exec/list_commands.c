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
static int	is_command(t_pipe_list *node);

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
		&& ft_strncmp(temp->args[0], "REDI", 4)
		&& ft_strncmp(temp->args[0], "HERE", 4))
		{
			if (is_path(temp->args[0]))
				temp->bin = ft_strdup(temp->args[0]);
			else
				temp->bin = path_finder(line, temp->args[0]);
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
		if (ft_strncmp(temp->args[0], "exit", 4) == 0)
			exit_builtin(line, temp, table);
		if (temp && !is_command(temp))
		{
			temp = temp->next ;
			continue ;
		}
		if (temp->bin && ft_strncmp(temp->bin, "builtin", 7) == 0
			&& temp->next == NULL)
			exec_builtins(temp, table);
		else
			init_fork(line, temp, table);
		temp = temp->next;
	}
	close_fds(line->list_cmds);
}

static int	is_command(t_pipe_list *node)
{
	return (ft_strncmp(node->args[0], "PIPE", 4)
		&& ft_strncmp(node->args[0], "REDO", 4)
		&& ft_strncmp(node->args[0], "REDA", 4)
		&& ft_strncmp(node->args[0], "REDI", 4)
		&& ft_strncmp(node->args[0], "HERE", 4));
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

//Vamos chamar a path_fider se a str não tiver ./ ou ../ (no caso se ela não for um path absoluto)
// Verificação com Access pra ver se ela é executavel, se for manda pro execve