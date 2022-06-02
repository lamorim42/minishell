/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 19:31:16 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/02 18:49:52 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_line(t_line *line)
{
	int	i;

	i = 0;
	free_list(line->list_cmds);
	ft_free_arr(line->tks);
	ft_free_arr(line->lex);
	ft_free_arr(line->ctks);
	free(line->str);
	while (line->cmds && line->cmds[i])
	{
		ft_free_arr(line->cmds[i]);
		i++;
	}
	free(line->cmds[i]);
	free(line->cmds);
}

void	free_sintax(t_line *line)
{
	printf("Syntax ERROR!\n");
	ft_free_arr(line->tks);
	ft_free_arr(line->lex);
	free(line->str);
}

void	free_list(t_pipe_list *list)
{
	t_pipe_list	*tmp;
	t_pipe_list	*aux;

	tmp = list;
	while (tmp)
	{
		aux = tmp->next;
		if (tmp->args[0] && ft_strncmp(tmp->args[0], "PIPE", 4)
			&& ft_strncmp(tmp->args[0], "REDO", 4)
			&& ft_strncmp(tmp->args[0], "REDA", 4)
			&& ft_strncmp(tmp->args[0], "VAR", 4))
			free(tmp->bin);
		free (tmp);
		tmp = aux;
	}
}
