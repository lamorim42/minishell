/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexical_analyzer.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/21 14:57:15 by lamorim           #+#    #+#             */
/*   Updated: 2022/05/25 20:12:06 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	**lexical_analyzer(t_line *line)
{
	int		i;
	char	**lex;

	i = 0;
	lex = (char **)malloc(sizeof(char *) * (line->tks_nbr + 1));
	while (line->tks[i])
	{
		if (!ft_strncmp(line->tks[i], "|", 1))
			lex[i] = ft_strdup("PIPE");
		else if (!ft_strncmp(line->tks[i], ">>", 2)
			&& ft_strlen(line->tks[i]) == 2)
			lex[i] = ft_strdup("REDA");
		else if (!ft_strncmp(line->tks[i], ">", 1))
			lex[i] = ft_strdup("REDO");
		else if (!ft_strncmp(line->tks[i], "$", 1))
			lex[i] = ft_strdup("VAR");
		else
			lex [i] = ft_strdup("WORD");
		i++;
	}
	lex[i] = NULL;
	return (lex);
}