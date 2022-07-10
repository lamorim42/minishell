/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 19:31:16 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/10 10:14:01 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_line(t_line *line)
{
	free_list(line->list_cmds);
	line->list_cmds = NULL;
	ft_free_arr(line->tks);
	line->tks = NULL;
	ft_free_arr(line->lex);
	line->lex = NULL;
	free(line->pid);
	line->pid = NULL;
	ft_free_arr(line->ctks);
	line->ctks = NULL;
	free(line->str);
	line->str = NULL;
}

void	free_sintax(t_line *line)
{
	printf("Syntax ERROR!\n");
	ft_free_arr(line->tks);
	line->tks = NULL;
	ft_free_arr(line->lex);
	line->lex = NULL;
	free(line->str);
	line->str = NULL;
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
			&& ft_strncmp(tmp->args[0], "VAR", 3)
			&& ft_strncmp(tmp->args[0], "REDI", 4)
			&& ft_strncmp(tmp->args[0], "HERE", 4))
		{
			free(tmp->bin);
			tmp->bin = NULL;
		}
		ft_free_arr(tmp->args);
		free (tmp);
		tmp = aux;
	}
}
