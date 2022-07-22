/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_list.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/22 14:22:31 by lamorim           #+#    #+#             */
/*   Updated: 2022/07/22 17:39:43 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	wait_child(t_line *line);
static void	update_status_code(t_line *line, t_hash_table **table);

void	exec_list(t_line *line, t_hash_table **table)
{
	t_pipe_list	*temp;

	temp = line->list_cmds;
	while (temp)
	{
		if (temp && !is_command(temp))
		{
			temp = temp->next ;
			continue ;
		}
		if (temp->bin && ft_strcmp_len(temp->bin, "builtin")
			&& temp->next == NULL)
			exec_builtins(temp, table);
		else
			init_fork(line, temp, table);
		temp = temp->next;
	}
	wait_child(line);
	update_status_code(line, table);
	close_fds(temp);
}

static void	wait_child(t_line *line)
{
	int	i;

	i = 0;
	while (line->pid && line->pid[i])
	{
		waitpid(line->pid[i], &(line->status_code), WUNTRACED);
		i++;
	}
}

static void	update_status_code(t_line *line, t_hash_table **table)
{
	char	*status;

	if (WIFEXITED(line->status_code))
		status = ft_itoa(WEXITSTATUS(line->status_code));
	else if (line->status_code == 1)
		status = ft_itoa(line->status_code);
	else if (WIFSIGNALED(line->status_code))
		status = ft_itoa(WTERMSIG(line->status_code) + 128);
	else
		status = ft_itoa(line->status_code);
	table_delete(table, "?");
	hash_insert(table, "?", status);
	free(status);
}
