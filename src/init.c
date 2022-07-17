/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 19:54:20 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/10 11:04:21 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	init_line(t_line *line)
{
	line->tks = NULL;
	line->status_code = 0;
	line->count_cmds = 0;
	line->tks_nbr = 0;
	line->lex = NULL;
	line->str = NULL;
	line->ctks = NULL;
	line->sig = 1;
	line->pid = NULL;
	line->list_cmds = NULL;
	ft_bzero(line->fd, 2);
}
