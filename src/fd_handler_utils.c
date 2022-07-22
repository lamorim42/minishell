/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_handler_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/22 19:02:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/22 19:26:11 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	check_valid_fd(t_line *line, t_pipe_list *list, char *flag);

void	open_fds(t_line *line)
{
	t_pipe_list	*list;

	list = line->list_cmds;
	while (list)
	{
		if (list->args[0][0] != '\0' && ft_strcmp_len(list->args[0], "REDI"))
			list->fd[0] = open(list->args[1], O_RDONLY);
		else if (list->args[0][0] != '\0'
		&& !ft_strncmp(list->args[0], "REDO", 4))
			check_valid_fd(line, list, "TRUNC");
		else if (list->args[0][0] != '\0'
		&& !ft_strncmp(list->args[0], "REDA", 4))
			check_valid_fd(line, list, "APPEND");
		else if (list->args[0][0] != '\0'
		&& !ft_strncmp(list->args[0], "PIPE", 4))
		{
			if (pipe(list->fd) != 0)
				error_msg("pipe", " error\n");
		}
		list = list->next;
	}
}

static void	check_valid_fd(t_line *line, t_pipe_list *list, char *flag)
{
	if (ft_strcmp_len(flag, "TRUNC"))
		list->fd[0] = open(list->args[1], O_RDWR | O_CREAT | O_TRUNC, 0644);
	else if (ft_strcmp_len(flag, "APPEND"))
	{
		list->fd[0] = open(list->args[1], O_RDWR | O_CREAT
				| O_APPEND, 0644);
	}
	if (list->fd[0] == -1)
	{
		perror(list->args[1]);
		free_line(line);
	}
}

void	close_std_fd(t_line *line)
{
	close(line->std_fd[IN]);
	close(line->std_fd[OUT]);
}
