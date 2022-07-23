/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/22 19:11:00 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 15:22:37 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

extern struct s_minishell	g_minishell;

void	signal_here(t_line *line)
{
	line->sig = 0;
	signal(SIGINT, sig_here);
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

void	signals(t_line *line)
{
	line->sig = 0;
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, SIG_IGN);
}

void	sig_handler(int sig)
{
	if (sig == SIGINT)
	{
		write(2, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}
