/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sintax_analysis.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/21 16:36:27 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/09 09:33:26 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	sintax_analysis(char **lex)
{
	int	i;

	i = 0;
	if (*lex && !ft_strncmp(lex[0], "PIPE", 4))
		return (0);
	while (lex[i])
	{
		if (!ft_strncmp(lex[i], "PIPE", 4)
			&& (lex[i + 1] != NULL && !ft_strncmp(lex[i + 1], "PIPE", 4)))
			return (0);
		if (!ft_strncmp(lex[i], "REDO", 4)
			&& (lex[i + 1] == NULL || ft_strncmp(lex[i + 1], "WORD", 4)))
			return (0);
		if (!ft_strncmp(lex[i], "REDA", 4)
			&& (lex[i + 1] == NULL || ft_strncmp(lex[i + 1], "WORD", 4)))
			return(0);
		if (!ft_strncmp(lex[i], "REDI", 4)
			&& (lex[i + 1] == NULL || ft_strncmp(lex[i + 1], "WORD", 4)))
			return(0);
		if (!ft_strncmp(lex[i], "HERE", 4)
			&& (lex[i + 1] == NULL || ft_strncmp(lex[i + 1], "WORD", 4)))
			return(0);
		i++;
	}
	return (1);
}
