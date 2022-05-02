/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creat_cmdlst.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/02 20:10:01 by lamorim           #+#    #+#             */
/*   Updated: 2022/05/02 20:51:17 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"
// ls ; pipe = 0 => 0 + (0 + 1)
// ls | echo ; pipe = 1 => 1 + (1+1)

char	**creat_cmd(t_line *line, char **ctks)
{
	char	**lex;
	int		count_pipes;
	int		count_words;
	int		i;

	lex = line->lex;
	i = 0;
	count_pipes = 0;
	count_words = 0;
	while (lex && lex[i])
	{
		if (!ft_strncmp(lex[i], "PIPE", 4))
			count_pipes++;
		i++;
	}
	line->cmds = (char **)malloc(sizeof(char **) * (count_pipes + (count_pipes + 1)));
	i = 0;
	while (lex && lex[i])
	{
		if (!ft_strncmp(lex[i], "WORD", 4))
			count_words++;
		if (!ft_strncmp(lex[i], "PIPE", 4))
		{
			// add a lista de cmds
		}
		i++;
	}
}

void	creat_cmd_list(t_line *line)
{
	char	**ctks;

	ctks = clean_tokens(line);
	line->cmds = creat_cmd(line, ctks);
}