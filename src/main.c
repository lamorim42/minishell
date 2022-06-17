/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:12:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/17 15:44:21 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	facade(t_line *line, t_hash_table **table);
static void	control_d(char *str);
static void	building_tokens(t_line *line);
static void	exec_pipe_line(t_line *line, t_hash_table **table);

int	main(int argc, char **argv, char **envp)
{
	t_line	line;
	t_hash_table	*table;

	table = NULL;

	if (argv[0] == NULL && argc > 1)
		return (1);
	init_line(&line);
	line.envp = envp;
	population_hash_table(&line, &table);
	facade(&line, &table);
	return (0);
}

static void	facade(t_line *line, t_hash_table **table)
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
				exec_pipe_line(line, table);
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

static void	exec_pipe_line(t_line *line, t_hash_table **table)
{

	//print_table(&table);
	creat_cmd(line, table);
	//int i = 0;
	/* while (line->cmds[i])
	{
		print_array("REDI:", line->cmds[i]);
		i++;
	} */
	population_linked_list(line);
	list_generation_bin(line);
	exec_list(line, table);
	add_history(line->str);
	free_line(line);
}


t_hash_table	*population_hash_table(t_line *line, t_hash_table **table)
{
	int	i;
	char	**var;

	i = 0;
	(*table) = create_table(SIZE_TABLE);
	while(line->envp[i])
	{
		var = ft_split(line->envp[i], '=');
		if (var[0] != NULL && var[1] != NULL)
			hash_insert(table, var[0], var[1]);
		i++;
	}
	return (*table);
}
