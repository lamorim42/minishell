/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creat_cmd_list.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/02 20:10:01 by lamorim           #+#    #+#             */
/*   Updated: 2022/05/19 17:23:02y lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	creat_cmd_list(t_line *line);
char	**copy_array(char **tokens, int size);
void	ft_free_array(char **mtx);
char	**make_redirect(char *red, char *file);

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
	line->cmds = ft_calloc((count_pipes * 2) + 3, (sizeof(char **)));
	if (line->cmds == NULL)
		return (NULL);
	i = 0;
	while (ctks && lex[i])
	{
		if ((!ft_strncmp(lex[i], "WORD", 4)) || (!ft_strncmp(lex[i], "VAR", 3)))
		{
			count_words++;
		}
		if (lex[i + 1] == NULL || !ft_strncmp(lex[i], "PIPE", 4)
			|| !ft_strncmp(lex[i], "REDO", 4)
			|| !ft_strncmp(lex[i], "REDA", 4))
		{
			if (count_words > 0)
			{
				line->cmds[j] = copy_array(ctks, count_words);
				ctks += count_words + (lex[i + 1] != NULL);
				j++;
			}
			if (!ft_strncmp(lex[i], "PIPE", 4))
			{
				line->cmds[j] = copy_array(&lex[i], 1);
				j++;
			}
			if (!ft_strncmp(lex[i], "REDO", 4))
			{
				if(!ft_strncmp(ctks[0], ">", 1))
					line->cmds[j] = make_redirect(lex[i], ctks[1]);
				else
					line->cmds[j] = make_redirect(lex[i], ctks[0]);
				ctks += 2;
				j++;
				i++;
			}
			if (!ft_strncmp(lex[i], "REDA", 4))
			{
				if(!ft_strncmp(ctks[0], ">>", 1))
					line->cmds[j] = make_redirect(lex[i], ctks[1]);
				else
					line->cmds[j] = make_redirect(lex[i], ctks[0]);
				ctks += 2;
				j++;
				i++;
			}
			count_words = 0;
		}
		i++;
	}
	line->cmds[j] = NULL;

	return (line->cmds);
}

void creat_cmd_list(t_line *line)
{
	line->ctks = clean_tokens(line);
	//print_array("CTKS", line->ctks);
	line->cmds = creat_cmd(line, line->ctks);
}

char **copy_array(char **tokens, int size)
{
	char **array_cpy;
	int len;
	int i;

	len = ft_array_len(tokens);
	array_cpy = (char **)malloc(sizeof(char *) * (len + 1));
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

char	**make_redirect(char *red, char *file)
{
	char	**node;

	node = (char **)malloc(sizeof(char *) * 3);
	node[0] = ft_strdup(red);
	node[1] = ft_strdup(file);
	node[2] = NULL;
	return (node);
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