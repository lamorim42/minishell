/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_fork.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:25:53 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/05 20:34:50 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_fork(t_line *line, int *i)
{

	if (pipe(line->fd) != 0)
		dprintf(2, "pipe error\n");
	line->pid = fork();
	if (line->pid == 0)
	{
		exec_path(line, i);
	}
	waitpid(line->pid, NULL, 0);
}