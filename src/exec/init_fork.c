/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_fork.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:25:53 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/06 17:09:57 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//static void	open_fd(t_pipe_list *list);
static void	update_status_code(t_line *line, t_hash_table **table);
void	init_fork(t_line *line, t_pipe_list *list, t_hash_table **table)
{
	//printf("%s\n", list->args[0]);
	//open_fd(list);
	if (list && (!ft_strncmp(list->args[0], "REDO", 4)
			|| !ft_strncmp(list->args[0], "REDA", 4)))
		close(list->fd[0]);
	else if (ft_strncmp(list->args[0], "PIPE", 4)
		&& ft_strncmp(list->args[0], "REDO", 4)
		&& ft_strncmp(list->args[0], "REDA", 4)
		&& ft_strncmp(list->args[0], "REDI", 4)
		&& ft_strncmp(list->args[0], "HERE", 4))
	{
		line->pid = fork();
		if (line->pid == 0)
			exec_path(line, list, table);
	}
	if (list->prev && !ft_strncmp(list->prev->args[0], "PIPE", 4))
	{
		close(list->prev->fd[0]);
		close(list->prev->fd[1]);
	}
	waitpid(line->pid, &(line->status_code), 0);
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

// static void	open_fd(t_pipe_list *list)
// {
// 	if (list->next && !ft_strncmp(list->next->args[0], "PIPE", 4))
// 	{
// 		if (pipe(list->next->fd) != 0)
// 			dprintf(2, "pipe error\n");
// 	}
// 	else if (list->next
// 		&& (!ft_strncmp(list->next->args[0], "REDO", 4)
// 			|| !ft_strncmp(list->next->args[0], "REDA", 4)))
// 	{
// 		list->next->fd[0] = open(list->next->args[1], O_WRONLY | O_CREAT
// 				| (O_TRUNC * (ft_strncmp(list->next->args[0], "REDO", 4) == 0))
// 				| (O_APPEND * (ft_strncmp(list->next->args[0],
// 							"REDA", 4) == 0)),
// 				0644);
// 		if (list->next->fd[0] == -1)
// 		{
// 			perror(list->next->args[1]);
// 			free_list(list);
// 		}
// 	}
// }
