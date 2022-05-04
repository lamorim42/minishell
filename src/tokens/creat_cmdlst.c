/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creat_cmdlst.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/02 20:10:01 by lamorim           #+#    #+#             */
/*   Updated: 2022/05/03 21:11:59 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
// ls ; pipe = 0 => 0 + (0 + 1)
// ls | echo ; pipe = 1 => 1 + (1+1)
void	creat_cmd_list(t_line *line);
char	**copy_array(char **tokens, int size);
void	ft_free_array(char **mtx);

char	***creat_cmd(t_line *line, char **ctks)
{
	char **lex;
	int count_pipes;
	int count_words;
	int i;
	int j;

	lex = line->lex;
	i = 0;
	j = 0;
	count_pipes = 0;
	count_words = 0;
	while (lex && lex[i])
	{
		if (!ft_strncmp(lex[i], "PIPE", 4))
			count_pipes++;
		i++;
	}
	line->cmds = ft_calloc((count_pipes + (count_pipes + 1)), (sizeof(char **)));
	if (line->cmds == NULL)
		return (NULL);
	i = 0;
	while (lex && lex[i])
	{
		if (!ft_strncmp(lex[i], "WORD", 4))
			count_words++;
		if (lex[i + 1] == NULL || !ft_strncmp(lex[i], "PIPE", 4))
		{
			line->cmds[j] = copy_array(ctks, count_words);
			j++;
			if (!ft_strncmp(lex[i], "PIPE", 4))
			{
				line->cmds[j] = copy_array(&lex[i], 1);
				j++;
			}
			ctks += count_words + (lex[i + 1] != NULL);
			count_words = 0;
		}
		i++;
	}
	line->cmds[j] = NULL;
	int counter = 0;
	while (counter < j)
	{
		print_array("cmds", line->cmds[counter]);
		counter++;
	}
	// Lembrar de colocar o | junto a esse array
	// para se colocar em uma linked list com separações
	// Assim ja teremos os blocos de comandos e podemos verificar se temos redirect;
	return (line->cmds);
}

void creat_cmd_list(t_line *line)
{
	line->ctks = clean_tokens(line);
	line->cmds = creat_cmd(line, line->ctks);
}

char **copy_array(char **tokens, int size)
{
	char **array_cpy;
	int len;
	int i;

	len = ft_array_len(tokens);
	array_cpy = malloc(sizeof(char *) * (len + 1));
	if (array_cpy == NULL)
		printf("!ERROR!\n");
	i = 0;
	while (i < size)
	{
		array_cpy[i] = ft_strdup(tokens[i]);
		i++;
	}
	array_cpy[i] = NULL;
	return (array_cpy);
}

void	ft_free_array(char **mtx)
{
	int i;

	i = 0;
	while ((mtx)[i])
	{
		free((mtx)[i]);
		i++;
	}
	free(mtx);
}