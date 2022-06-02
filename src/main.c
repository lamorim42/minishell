/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:12:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/02 19:41:51 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	facade(t_line *line);
static void	control_d(char *str);
static void	building_tokens(t_line *line);
static void	exec_pipe_line(t_line *line);

int	main(int argc, char **argv, char **envp)
{
	t_line	line;

	if (argv[0] == NULL && argc > 1)
		return (1);
	init_line(&line);
	line.envp = envp;
	facade(&line);
	return (0);
}

static void	facade(t_line *line)
{
	while (1)
	{
		signals(line);
		line->str = readline("miau> ");
		control_d(line->str);
		if (line->str != NULL && ft_strlen(line->str) > 0)
		{
			building_tokens(line);
			if (!sintax_analysis(line->lex))
				free_sintax(line);
			else
				exec_pipe_line(line);
		}
		else
			free(line->str);
	}
}

static void	control_d(char *str)
{
	if (str == NULL)
	{
		write(2, "exit\n", 6);
		exit(0);
	}
}

static void	building_tokens(t_line *line)
{
	line->tks_nbr = count_tks(line->str);
	line->tks = tokenizer(line);
	line->lex = lexical_analyzer(line);
}

static void	exec_pipe_line(t_line *line)
{
	creat_cmd(line);
	int i = 0;
	while (line->cmds[i])
	{
		print_array("REDI:", line->cmds[i]);
		i++;
	}
	population_linked_list(line);
	list_generation_bin(line);
	exec_list(line);
	add_history(line->str);
	free_line(line);
}
