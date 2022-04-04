/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 09:28:09 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/04 12:18:23 by dmonteir         ###   ########.fr       */
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
		if (j < 1)
		{
			pipe(line->fd);
			get_fd(line, j);
		}
		if (line->array_cmds[i][0][0] == '|')
		{
			i++;
		}
		if (i > 0 && line->array_cmds[i - 1][0][0] == '|')
		{
			printf("if_dupin: %d\n", i);
			dup_in(line, j - 1);
		}
		if (line->array_cmds[i + 1] && line->array_cmds[i + 1][0][0] == '|')
		{
			printf("if_dupout: %d\n", i);
			dup_out(line, j);
		}
		//Verificar se é cmd ou builtin
		ft_fork(line, i);
		i++;
		j++;
	}
	i = 0;
	while(line->array_pid[i])
	{
		waitpid(line->array_pid[i], NULL, 0);
		i++;
	}
}

void	get_fd(t_line *line, int i)
{
	line->array_fd[i] = malloc(sizeof(int *) * 3);
	line->array_fd[i][0] = line->fd[0];
	line->array_fd[i][1] = line->fd[1];
	line->array_fd[i][2] = '\0';
}

void	dup_in(t_line *line, int j)
{
	printf("dup_in j: %d\n", j);
	dup2(STDIN_FILENO, line->array_fd[j][0]);
	close(line->array_fd[j][0]);
	close(line->array_fd[j][1]);
}

void	dup_out(t_line *line, int j)
{
	//abrir heredoc se caso for um ultimo pipe
	printf("dup_out j: %d\n", j);
	dup2(STDOUT_FILENO, line->array_fd[j][1]);
	close(line->array_fd[j][0]);
	close(line->array_fd[j][1]);
}

void	ft_fork(t_line *line, int i)
{
	int	pid;

	pid = fork();
	//printf("oii");
	get_pid(line, pid, i);
	if (pid < 0)
		return_error();
	if (pid == 0 && line->bin[i])
	{
		exec_cmd(line, i);
	}
}

void	get_pid(t_line *line, int pid, int i)
{
	printf("pid: %d\n", pid);
	if (pid > 0)
		line->array_pid[i] = pid;
}

void	exec_cmd(t_line *line, int i)
{
	static int	j;

	j = 0;
	if (line->array_cmds[j][0][0] == '|')
		j++;

	if (execve(line->bin[i], line->array_cmds[j++], line->envp) == -1)
	{
		printf("line->bin[i]: %s\n", line->bin[i]);
		perror("command not found");
	}
}
