/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 19:54:20 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/25 12:14:57 by lamorim          ###   ########.fr       */
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
