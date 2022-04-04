/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 09:28:09 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/01 16:57:20 by lamorim          ###   ########.fr       */
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
	int	pid;

	pid = fork();

	if (pid < 0)
		return_error();
	if (pid == 0)
		ft_dup_pipe(line);
	else
		waitpid(pid, NULL, 0);
}

void	ft_dup_pipe(t_line *line)
{
	dup2(STDOUT_FILENO, line->fd[1]);
	close(line->fd[1]);
	close(line->fd[0]);
	exec_cmd(line);
}