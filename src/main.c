/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:12:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 16:47:31 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

struct s_minishell	g_minishell;

static void	main_loop(t_line *line, t_hash_table **table);
static void	control_d(char *str, t_hash_table **table);
static int	is_a_comment(char *str);

int	main(int argc, char **argv, char **envp)
{
	t_line			line;
	t_hash_table	*table;

	table = NULL;
	if (argv[0] == NULL && argc > 1)
		return (1);
	init_line(&line);
	line.envp = envp;
	population_hash_table(&line, &table);
	g_minishell.line = &line;
	g_minishell.table = table;
	main_loop(&line, &table);
	return (0);
}

static void	main_loop(t_line *line, t_hash_table **table)
{
	line->std_fd[IN] = dup(STDIN_FILENO);
	line->std_fd[OUT] = dup(STDOUT_FILENO);
	while (TRUE)
	{
		signals(line);
		line->str = readline("miau> ");
		control_d(line->str, table);
		if (!verify_quots(line->str))
		{
			free(line->str);
			printf("Syntax ERROR!\n");
			continue ;
		}
		if (is_a_comment(line->str))
		{
			free(line->str);
			continue ;
		}
		facade(line, table);
		dup2(line->std_fd[IN], STDIN_FILENO);
		dup2(line->std_fd[OUT], STDOUT_FILENO);
	}
	close_std_fd(line);
}

static void	control_d(char *str, t_hash_table **table)
{
	if (str == NULL)
	{
		free_table(table);
		write(2, "exit\n", 6);
		close_std_fd(g_minishell.line);
		exit(0);
	}
}

int	file_exists(int fd)
{
	struct stat	buffer;

	return (fstat (fd, &buffer) == 0);
}

static int	is_a_comment(char *str)
{
	int	i;

	i = 0;
	while (str[i] && str[i] == ' ')
		i++;
	if (str[i] == '#')
		return (TRUE);
	return (FALSE);
}
