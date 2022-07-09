/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:42:50 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/09 11:47:35 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	sig_handler(int sig)
{
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

//dar o status_code no final do filho

static void	sig_parent(int sig)
{
	if (sig == 3)
		return ;
	else
	{
		write(2, "\n", 1);
		rl_on_new_line();
	}
}

static void	sig_child(int sig)
{
	if (sig == 2)
	{
		//printf("\n");
		write(2, "\n", 1);
		//g_status_code = 130;
		exit(130);
	}
	else if (sig == 3)
	{
		printf("Quit\n");
		//g_status_code = 131;
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
	signal(SIGQUIT, sig_parent);
}

//lidar cat|cat|ls abrindo os pipes e executando
