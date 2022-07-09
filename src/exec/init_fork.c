/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_fork.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:25:53 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/09 12:00:28 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_status_code(t_line *line, t_hash_table **table);
void	init_fork(t_line *line, t_pipe_list *list, t_hash_table **table)
{
	if (list && (!ft_strncmp(list->args[0], "REDO", 4)
			|| !ft_strncmp(list->args[0], "REDA", 4)))
		close(list->fd[0]);
	else if (ft_strncmp(list->args[0], "PIPE", 4)
		&& ft_strncmp(list->args[0], "REDO", 4)
		&& ft_strncmp(list->args[0], "REDA", 4)
		&& ft_strncmp(list->args[0], "REDI", 4)
		&& ft_strncmp(list->args[0], "HERE", 4))
	{
		signals_parent(line);
		line->pid = fork();
		if (line->pid == 0)
		{
			signals_child(line);
			exec_path(line, list, table);
		}

	}
	if (list->prev && !ft_strncmp(list->prev->args[0], "PIPE", 4))
	{
		close(list->prev->fd[0]);
		close(list->prev->fd[1]);
	}
	waitpid(line->pid, &(line->status_code), WUNTRACED);
	//Não estamos recebendo o status_code vindo do processo filho
	update_status_code(line, table);
}

static void	update_status_code(t_line *line, t_hash_table **table)
{
	char	*status;

	status = ft_itoa(WEXITSTATUS(line->status_code));
	table_delete(table, "?");
	hash_insert(table, "?", status);
	free(status);
}
