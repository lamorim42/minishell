/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:12:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/27 20:27:54 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	facade(t_line *line);


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
			line->lex = lexical_analyzer(line);
			if (!sintax_analysis(line->lex))
			{
				printf("Syntax ERROR!\n");
				ft_free_arr(line->tks);
				ft_free_arr(line->lex);
				free(line->str);
			}
			else
			{
				creat_cmd_list(line);
				population_linked_list(line);
				//se é builtin ou não, passar algo pro bin, se não for
				//se for a flag de bin vai chamar o builtin se não é exec
				list_generation_bin(line);
				exec_list(line);
				add_history(line->str);
				//print_array("ctks", line->ctks);
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
	ft_free_arr(line->tks);
	ft_free_arr(line->lex);
	ft_free_arr(line->ctks);
	free(line->str);
	while (line->cmds && line->cmds[i])
	{
		ft_free_arr(line->cmds[i]);
		i++;
	}
	free(line->cmds[i]);
	free(line->cmds);
}
