/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 19:54:20 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/12 19:40:10 by lamorim          ###   ########.fr       */
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
	ft_bzero(line->fd, 2);
}
