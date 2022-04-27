/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_fork.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:25:53 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/25 20:34:23 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_fork(t_line *line)
{
	line->pid = fork();
	if (line->pid == 0)
		exec_path(line);
	waitpid(line->pid, NULL, 0);
}