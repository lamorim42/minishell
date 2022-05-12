/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:04:40 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/11 20:13:44 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exec_path(t_line *line, t_pipe_list *list)
{

	if (!ft_strncmp(list->next->args[0], "PIPE", 4))
	{
		dup2(line->fd[0], STDIN_FILENO);
		close(line->fd[0]);
		close(line->fd[1]);
	} else if (!ft_strncmp(list->prev->args[0], "PIPE", 4))
	{
		printf("%s\n", "gatinho");
		dup2(line->fd[1], STDOUT_FILENO);
		close(line->fd[1]);
		close(line->fd[0]);
	}
	//print_array("cmds", line->ctks)

	if (!execve(list->bin, list->args, line->envp))
		free_line(line);
}
