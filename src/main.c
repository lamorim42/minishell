/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:12:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/21 17:58:01 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	facade(t_line *line);
void	free_line(t_line *line);
void	free_array(char **mtx);

int	main()
{
	 t_line line;
	facade(&line);

	return (0);
}

void	facade(t_line *line)
{
	while (1)
	{
		line->str = readline("miau> ");
		line->tks_nbr = count_tks(line->str);
		line->tks = tokenizer(line);
		line->lex = lexical_analyzer(line);
		if (!sintax_analysis(line->lex))
		{
			printf("Syntax ERROR!\n");
		}
		else
			printf("%s\n", line->str);
		line->cmds = clean_tokens(line);
		free_line(line);
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

