/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 09:28:09 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/06 10:06:51 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exec_cmd_pipe(t_line *line)
{
	int	i;
	int	j;

	j = 0;
	i = 0;
	while (line->array_cmds[i])
	{
		if (j < line->nb_pipes)
			pipe(line->fd);
		if (line->array_cmds[i][0][0] == '|')
			i++;
		ft_fork(line, i, j);
		if (line->nb_pipes > 0 && line->array_cmds[i + 1] && line->array_cmds[i + 1][0][0] == '|')
			dup_in(line);
		else if (line->nb_pipes > 0 && line->array_cmds[i + 1] == NULL)
			dup_out(line);
		//Verificar se é cmd ou builtin
		i++;
		j++;
	}
}

void	dup_in(t_line *line)
{
	dup2(line->fd[0], STDIN_FILENO);
	close(line->fd[0]);
	close(line->fd[1]);
}

void	dup_out(t_line *line)
{
	//abrir heredoc se caso for um ultimo pipe
	dup2(line->fd[1], STDOUT_FILENO);
	close(line->fd[1]);
	close(line->fd[0]);
}

void	ft_fork(t_line *line, int i, int j)
{
	int	pid;

	pid = fork();
	if (pid < 0)
		return_error();
	if (pid == 0)
	{
		exec_cmd(line, i, j);
	}
	waitpid(pid, NULL, 0);
}

void	exec_cmd(t_line *line, int i, int j)
{
	if (line->nb_pipes > 0)
	{
		close(line->fd[0]);
		dup2(line->fd[1], STDOUT_FILENO);
		close(line->fd[1]);
	}

	if (execve(line->bin[j], line->array_cmds[i], line->envp) == -1)
	{
		perror("command not found");
	}
}
