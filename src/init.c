/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 19:54:20 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/11 20:21:21 by dmonteir         ###   ########.fr       */
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
}
