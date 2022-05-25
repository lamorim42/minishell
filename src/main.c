/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:12:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/25 11:56:39 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	facade(t_line *line);
void	free_array(char **mtx);

int	main(int argc, char **argv, char **envp)
{
	t_line line;

	if(argv[0] == NULL && argc > 1)
		return 1;
	init_line(&line);
	line.envp = envp;
	facade(&line);
	return (0);
}

void	facade(t_line *line)
{
	while (1)
	{
		signals(line);
		line->str = readline("miau> ");
		if (line->str == NULL)
		{
			write(2, "exit\n", 6);
			exit(0);
		}
		if (line->str != NULL && ft_strlen(line->str) > 0)
		{
			line->tks_nbr = count_tks(line->str);
			line->tks = tokenizer(line);
			//print_array("tks", line->tks);
			line->lex = lexical_analyzer(line);
			if (!sintax_analysis(line->lex))
			{
				printf("Syntax ERROR!\n");
				free_array(line->tks);
				free_array(line->lex);
				free(line->str);
			}
			else
			{
				creat_cmd_list(line);
				population_linked_list(line);
				list_generation_bin(line);
				exec_list(line);
				add_history(line->str);
				free_line(line);
			}
		}
		else
		{
			free(line->str);
		}
	}
}

void free_line(t_line *line)
{
	int i;
	i = 0;

	free_list(line->list_cmds);
	free_array(line->tks);
	free_array(line->lex);
	free_array(line->ctks);
	free(line->str);
	while (line->cmds && line->cmds[i])
	{
		free_array(line->cmds[i]);
		i++;
	}
	free(line->cmds[i]);
	free(line->cmds);
}

void free_array(char **mtx)
{
	int i;

	i = 0;
	while (mtx[i])
	{
		free(mtx[i]);
		i++;
	}
	free(mtx);
}
