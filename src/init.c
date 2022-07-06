/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 19:54:20 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/05 20:21:10 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	init_line(t_line *line)
{
	line->tks = NULL;
	line->tks_nbr = 0;
	line->lex = NULL;
	line->cmds = NULL;
	line->str = NULL;
	line->ctks = NULL;
	line->sig = 1;
	line->pid = 0;
	ft_bzero(line->fd, 2);
}

void	init_aux(struct s_aux *aux, t_line *line)
{
	aux->count_pipes = 0;
	aux->count_words = 0;
	aux->i = 0;
	aux->j = 0;
	aux->ctks = line->ctks;
	aux->lex = line->lex;
}
