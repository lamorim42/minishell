/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:42:50 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/22 19:13:45 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	sig_parent(int sig)
{
	if (sig == SIGINT)
	{
		g_minishell.line->status_code = 130;
		write(2, "\n", 1);
		rl_on_new_line();
	}
}

void	sig_write(int sig)
{
	if (sig == SIGQUIT)
		write(2, "Quit\n", 5);
}

void	signal_ignore(t_line *line)
{
	line->sig = 0;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void	signals_parent(t_line *line)
{
	line->sig = 0;
	signal(SIGINT, sig_parent);
	signal(SIGQUIT, sig_write);
}
