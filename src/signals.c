/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 20:42:50 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/18 16:35:59 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	sig_handler(int sig)
{
	if (sig == 2)
	{
		write(2, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

static void	sig_parent(int sig)
{
	if (sig == 2)
	{
		write(2, "\n", 1);
		rl_on_new_line();
	}
}

static void	sig_child(int sig)
{
	if (sig == 2)
		exit(130);
	else if (sig == 3)
	{
		write(2, "Quit\n", 5);
		exit(131);
	}
}

void	sig_write(int sig)
{
	if (sig == 3)
		write(2, "Quit\n", 5);
}

void	signals(t_line *line)
{
	line->sig = 0;
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, SIG_IGN);
}

void	signal_ignore(t_line *line)
{
	line->sig = 0;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void	sig_here(int sig)
{
	printf("\n");
	if (sig == SIGINT)
	{
		close_std_fd(g_minishell.line);
		free_line(g_minishell.line);
		free_table(&(g_minishell.table));
		exit(130);
	}
}

void	signal_here(t_line *line)
{
	line->sig = 0;
	signal(SIGINT, sig_here);
	//signal(SIGQUIT, SIG_IGN);
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
	signal(SIGQUIT, sig_write);
}
