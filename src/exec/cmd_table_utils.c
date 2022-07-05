/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_table_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 21:13:02 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/05 18:49:34 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**make_redirect(char *red, char *file);

void	update_cmd_table(t_line *line, struct s_aux *aux)
{
	if (aux->lex[aux->i + 1] == NULL || !ft_strncmp(aux->lex[aux->i], "PIPE", 4)
		|| !ft_strncmp(aux->lex[aux->i], "REDO", 4)
		|| !ft_strncmp(aux->lex[aux->i], "REDA", 4)
		|| !ft_strncmp(aux->lex[aux->i], "REDI", 4)
		|| !ft_strncmp(aux->lex[aux->i], "HERE", 4))
	{
		if (aux->count_words > 0)
		{
			line->cmds[aux->j] = copy_array(aux->ctks, aux->count_words);
			aux->ctks += aux->count_words + (aux->lex[aux->i + 1] != NULL);
			aux->j++;
		}
		if (!ft_strncmp(aux->lex[aux->i], "PIPE", 4))
		{
			line->cmds[aux->j] = copy_array(&aux->lex[aux->i], 1);
			aux->j++;
		}
		if (!ft_strncmp(aux->lex[aux->i], "REDO", 4))
			update_red(line, aux, ">");
		if (!ft_strncmp(aux->lex[aux->i], "REDA", 4))
			update_red(line, aux, ">>");
		if (!ft_strncmp(aux->lex[aux->i], "REDI", 4))
			update_red(line, aux, "<");
		if (!ft_strncmp(aux->lex[aux->i], "HERE", 4))
			update_red(line, aux, "<<");
		aux->count_words = 0;
	}
}

void	update_red(t_line *line, struct s_aux *aux, char *red)
{
	if (!ft_strncmp(aux->ctks[0], red, ft_strlen(red)))
		line->cmds[aux->j] = make_redirect(aux->lex[aux->i], aux->ctks[1]);
	else
		line->cmds[aux->j] = make_redirect(aux->lex[aux->i], aux->ctks[0]);
	aux->ctks += 2;
	aux->j++;
	aux->i++;
}

static char	**make_redirect(char *red, char *file)
{
	char	**node;

	node = (char **)malloc(sizeof(char *) * 3);
	node[0] = ft_strdup(red);
	node[1] = ft_strdup(file);
	node[2] = NULL;
	return (node);
}
