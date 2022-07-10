/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:42:50 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/09 19:40:21 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	sig_handler(int sig)
{
	if (sig == 3)
		return ;
	else if (sig == 2)
	{
		write(2, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

//dar o status_code no final do filho

static void	sig_parent(int sig)
{
	if (sig == 3)
		write(2, "oi\n", 1);
	else if (sig == 2)
	{
		write(2, "\n", 1);
		rl_on_new_line();
	}
}

static void	sig_child(int sig)
{
	if (sig == 2)
	{
		write(2, "oi\n", 1);
		exit(130);
	}
	else if (sig == 3)
	{
		rl_on_new_line();
		rl_replace_line("Quit\n", 5);
		rl_redisplay();
		exit(131);
	}
}

void	signals(t_line *line)
{
	line->sig = 0;
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, SIG_IGN);
}

void	signals_child(t_line *line)
{
	line->sig = 0;
	signal(SIGINT, sig_child);
	signal(SIGQUIT, sig_child);
}

void	signals_parent(t_line *line)
{
	line->sig = 0;
	signal(SIGINT, sig_parent);
	signal(SIGQUIT, SIG_IGN);
}

//lidar cat|cat|ls abrindo os pipes e executando
