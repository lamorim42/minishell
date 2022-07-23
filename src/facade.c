/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   facade.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/22 18:48:58 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 17:08:49 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	alloc_commands(t_line *line);
static void	exec_pipe_line(t_line *line, t_hash_table **table);

void	facade(t_line *line, t_hash_table **table)
{
	if (line->str != NULL && ft_strlen(line->str) > 0)
	{
		building_tokens(line);
		if (!sintax_analysis(line->lex))
			free_sintax(line);
		else
			exec_pipe_line(line, table);
	}
	else
		free(line->str);
}

static void	exec_pipe_line(t_line *line, t_hash_table **table)
{
	int	pid;

	expand_var(line, *table);
	line->ctks = clean_tokens(line);
	creat_list_cmd(line);
	creat_here_doc(line->list_cmds);
	pid = fork();
	signal_ignore(line);
	if (pid == 0)
	{
		signal_here(line);
		here_doc_verification(line);
		close_std_fd(line);
		free_line(line);
		free_table(table);
		exit(0);
	}
	waitpid(pid, &(line->status_code), 0);
	close_here_doc(line->list_cmds);
	list_generation_bin(line, table);
	alloc_commands(line);
	open_fds(line);
	exec_list(line, table);
	add_history(line->str);
	free_line(line);
}

static void	alloc_commands(t_line *line)
{
	t_pipe_list	*temp;

	temp = line->list_cmds;
	line->pid_index = 0;
	while (temp)
	{
		if (is_command(temp))
			line->count_cmds++;
		temp = temp->next;
	}
	line->pid = ft_calloc((line->count_cmds + 1), sizeof(int));
	if (!line->pid)
		return ;
}
