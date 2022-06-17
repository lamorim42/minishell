/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creat_cmd_table.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/02 20:10:01 by lamorim           #+#    #+#             */
/*   Updated: 2022/06/16 11:10:19 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	counter_pipes(int *count_pipes, char **temp_lex)
{
	int	i;

	i = 0;
	while (temp_lex && temp_lex[i])
	{
		if (!ft_strncmp(temp_lex[i], "PIPE", 4))
			(*count_pipes)++;
		i++;
	}
}

static char	***creat_cmd_table(t_line *line)
{
	struct s_aux	aux;

	init_aux(&aux, line);
	counter_pipes(&aux.count_pipes, aux.lex);
	line->cmds = ft_calloc((aux.count_pipes * 2) + 3, (sizeof(char **)));
	if (line->cmds == NULL)
		return (NULL);
	while (aux.ctks && aux.lex[aux.i])
	{
		if ((!ft_strncmp(aux.lex[aux.i], "WORD", 4))
			|| (!ft_strncmp(aux.lex[aux.i], "VAR", 3)))
			aux.count_words++;
		update_cmd_table(line, &aux);
		aux.i++;
	}
	line->cmds[aux.j] = NULL;
	return (line->cmds);
}

void	creat_cmd(t_line *line, t_hash_table **table)
{
	line->ctks = clean_tokens(line, (*table));
	line->cmds = creat_cmd_table(line);
}
