/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:42:50 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/26 20:05:48 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	sig_handler(int sig);

void	sig_handler(int sig)
{
	// printf("%d\n", sig);
	if (sig == 3)
		return ;
	else
	{
		write(2, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void	signals(t_line *line)
{
	line->tks_nbr = 1;
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, sig_handler);
}