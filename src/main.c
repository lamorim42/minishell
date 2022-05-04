/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:12:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/03 19:37:05 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	facade(t_line *line);
void	free_array(char **mtx);

int	main(int argc, char **argv, char **envp)
{
	t_line line;

	if (argc > 1)
		return (1);
	init_line(&line);
	dprintf(2, "só um argv %s\n", argv[0]);
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
			exit(1);
		}
		if (line->str != NULL && ft_strlen(line->str) > 0)
		{
			line->tks_nbr = count_tks(line->str);
			line->tks = tokenizer(line);
			line->lex = lexical_analyzer(line);
			if (!sintax_analysis(line->lex))
			{
				printf("Syntax ERROR!\n");
			}
			creat_cmd_list(line);
			line->bin = path_finder(line);
			init_fork(line);
			//printf("%s\n", line->cmds[0]);
			free_line(line);
		}
	}
}

void free_line(t_line *line)
{
	free_array(line->tks);
	free_array(line->lex);
	free(line->str);
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
