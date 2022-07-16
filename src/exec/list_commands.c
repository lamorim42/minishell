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
static void	update_status_code(t_line *line, t_hash_table **table);

void	list_generation_bin(t_line *line)
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
	int	i;
	t_pipe_list	*temp;

	i = 0;
	temp = line->list_cmds;
	while (temp)
	{
		if (ft_strcmp(temp->args[0], "exit"))
			exit_builtin(line, temp, table);
		if (temp && !is_command(temp))
		{
			temp = temp->next ;
			continue ;
		}
		if (temp->bin && ft_strcmp_len(temp->bin, "builtin"))
				exec_builtins(temp, table);
		else
			init_fork(line, temp, table);
		temp = temp->next;
	}
	while (line->pid && line->pid[i])
	{
		waitpid(line->pid[i], &(line->status_code), 0);
		i++;
	}
	update_status_code(line, table);
	close_fds(temp);
}

int	is_command(t_pipe_list *node)
{
	return (ft_strncmp(node->args[0], "PIPE", 4)
		&& ft_strncmp(node->args[0], "REDO", 4)
		&& ft_strncmp(node->args[0], "REDA", 4)
		&& ft_strncmp(node->args[0], "REDI", 4)
		&& ft_strncmp(node->args[0], "HERE", 4));
}

void	exec_builtins(t_pipe_list *node, t_hash_table **table)
{
	find_input(node);
	find_output(node);
	if (ft_strcmp_len(node->args[0], "echo"))
		echo_builtin(node);
	else if (ft_strcmp_len(node->args[0], "cd"))
		cd_builtin(node, table);
	else if (ft_strcmp_len(node->args[0], "pwd"))
		pwd_builtin(node, table);
	else if (ft_strcmp_len(node->args[0], "export"))
		export_builtin(node, table);
	else if (ft_strcmp_len(node->args[0], "unset"))
		unset_builtin(node, table);
	else if (ft_strcmp_len(node->args[0], "env"))
		env_builtin(node, table);
	else
		error_msg(node->args[0], ": command not found\n");
}


static void	update_status_code(t_line *line, t_hash_table **table)
{
	char	*status;

	if (WIFEXITED(line->status_code))
		status = ft_itoa(WEXITSTATUS(line->status_code));
	else
		status = ft_itoa(WTERMSIG(line->status_code) + 128);
	table_delete(table, "?");
	hash_insert(table, "?", status);
	free(status);
}

//Vamos chamar a path_fider se a str não tiver ./ ou ../ (no caso se ela não for um path absoluto)
// Verificação com Access pra ver se ela é executavel, se for manda pro execve