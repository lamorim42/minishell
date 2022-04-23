/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:12:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/23 12:24:00 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	facade(t_line *line);
void	free_line(t_line *line);
void	free_array(char **mtx);

int	main(int argc, char **argv, char **envp)
{
	t_line line;

	if (argc > 1)
		return (1);
	dprintf(2, "só um argv %s\n", argv[0]);
	line.envp = envp;
	facade(&line);
	return (0);
}

void	facade(t_line *line)
{
	while (1)
	{
		line->str = readline("miau> ");
		if (ft_strlen(line->str) > 0)
		{
			line->tks_nbr = count_tks(line->str);
			line->tks = tokenizer(line);
			line->lex = lexical_analyzer(line);
			if (!sintax_analysis(line->lex))
			{
				printf("Syntax ERROR!\n");
			}
			line->cmds = clean_tokens(line);
			line->bin = path_finder(line);
			printf("%s\n", line->cmds[0]);
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

