/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 08:29:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/23 16:28:14 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_line(t_line *line);

int	main(void)
{
	t_line	line;
	int		i;

	i = 0;
	init_line(&line);
	line.pipeline = readline("miau> ");
	line.tks = token(line.pipeline);
	line.lex = tokens_classification(line.tks);
	rules_grammar(line.lex);

	while (line.tks[i])
	{
		printf("toke = %s <=> lex = %s\n", line.tks[i], line.lex[i]);
		i++;
	}

	free(line.pipeline);
	ft_free_array(&(line.tks));
	ft_free_array(&(line.lex));
	return (0);
}

void	init_line(t_line *line)
{
	line->pipeline = NULL;
	line->tks = NULL;
	line->lex = NULL;
}

/* char	**table_cmds(char **tokens, int size)
{
	char	**temp;
	char	**table;
	int		i;

	i = 0;
	temp = tokens;
	if (size == 0)
	{
		table = tokens;
		return (table);
	}
	table = malloc(sizeof(char *) * (size + 2));
	while (*temp)
	{
		if (ft_strchr(*temp, '|'))
		{
			temp++;
		}
		else
		{
			table[i] = ft_strdup(*temp);
			i++;
			temp++;
		}
	}
	table[i] = NULL;
	return (table);
}

char	**table_red(char **tokens, int size)
{
	char	**temp;
	char	**array_red;
	int		i;

	temp = tokens;
	i = 0;

	array_red = malloc(sizeof(char *) * (size + 1));
	while(*temp)
	{
		if (ft_strchr(*temp, '|') || ft_strchr(*temp, '>'))
		{
			array_red[i] = ft_strdup(*temp);
			i++;
			temp++;
		} else
			temp++;
	}
	array_red[i] = NULL;
	temp = array_red;
	while (*temp)
		temp++;
	return (array_red);
} */

void	return_error(void)
{
	printf("error!\n");
}
