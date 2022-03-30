/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 08:29:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/30 16:08:02 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char **argv, char **envp)
{
	t_line	line;
	int	i;

	i = 0;
	/* if (argc != 1)
		printf("Use ./minishell"); */
	init_line(&line, argc, argv, envp);
	line.pipeline = readline("miau> ");
	if (ft_strlen(line.pipeline))
	{
		count_pipe(&line);
		line.tks = token(line.pipeline);
		line.lex = lexical_analysis(line.tks);
		syntax_analisys(line.lex);
		// expansão
		create_cmd(&line);
		str_cmd(&line);
		split_path(&line);
		line.bin = check_path(&line);

		free(line.pipeline);
		ft_free_array(line.bin);
		ft_free_array(line.cmds_table);
		ft_free_array(line.path);
		ft_free_array(line.tks);
		ft_free_array(line.lex);
		ft_free_arrcmds(line.array_cmds);
	}
	return (0);
}

void	init_line(t_line *line, int argc, char **argv, char **envp)
{
	line->pipeline = NULL;
	line->tks = NULL;
	line->lex = NULL;
	line->nb_pipes = 0;
	line->nb_cmds = 0;
	line->array_cmds = NULL;
	line->argv = argv;
	line->envp = envp;
	line->argc = argc;
	line->path = NULL;
	line->bin = NULL;
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

void return_error(void)
{
	printf("error!\n");
}
