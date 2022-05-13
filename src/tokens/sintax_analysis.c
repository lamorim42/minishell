/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sintax_analysis.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/21 16:36:27 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/13 19:42:18 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	sintax_analysis(char **lex)
{
	int	i;

	i = 0;
	if (!ft_strncmp(lex[0], "PIPE", 4))
		return(0);
	while(lex[i])
	{
		if (!ft_strncmp(lex[i], "REDO", 4)
		&& (lex[i + 1]) && !ft_strncmp(lex[i + 1], "PIPE", 4))
			return(0);
		i++;
	}
	return (1);
}