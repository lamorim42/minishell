/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sintax_analysis.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/21 16:36:27 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/21 16:58:04 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	sintax_analysis(char **lex)
{
	int		i;
	i = 0;
	if (!ft_strncmp(lex[0], "PIPE", 4))
		return(0);
	/*while(lex[i])
	{
		if (ft_strncmp(lex[i + 1], "PIPE", 4))
			return(0);
		i++;
	} */
	return (1);
}