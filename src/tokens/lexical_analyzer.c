/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexical_analyzer.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/21 14:57:15 by lamorim           #+#    #+#             */
/*   Updated: 2022/04/21 16:03:58 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	**lexical_analyzer(t_line *line)
{
	char	**lex;
	int		len;

	lex = (char **)malloc(sizeof(char *) * 2);
	lex[0] = "WORD";
	lex[1] = NULL;
	return (lex);
}