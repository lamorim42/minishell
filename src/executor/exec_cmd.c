/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 09:28:09 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/01 10:51:09 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exec_cmd(t_line *line)
{
	static int	i;
	static int	j;

	i = 0;
	j = 0;
	if (line->array_cmds[j][0][0] == '|')
		j++;
	if (execve(line->bin[i++], line->array_cmds[j++], line->envp) == -1)
		perror("command not found");
}

void	ft_fork(t_line *line)
{
	int	cmd;

	cmd = fork();

	if (cmd < 0)
		return_error();
	if (cmd == 0)
		exec_cmd(line);
	else
		waitpid(cmd, NULL, 0);
}