/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:04:40 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/10 19:14:35 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exec_path(t_line *line, int *i)
{

	dprintf(2, "%d\noiiii", *i);
	if ((*i) < line->tks_nbr)
	{
		if (*i == 0)
		{
			close(line->fd[0]);
			dup2(line->fd[1], STDOUT_FILENO);
			close(line->fd[1]);
		}
		else
		{
			dup2(line->fd[0], STDIN_FILENO);
			close(line->fd[1]);
			close(line->fd[0]);
		}
	}
	//print_array("cmds", line->ctks);
	if (*i == 0)
	{
		close(line->fd[0]);
		if (!execve(line->bin, line->cmds[0], line->envp))
			free_line(line);
	} else
	{
		if (!execve(line->bin, line->cmds[2], line->envp))
			free_line(line);
	}
}
