/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:12:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/21 15:48:24 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main ()
{
	t_line	line;
	char	**nbr;

	line.str = readline("miau> ");
	line.tks_nbr = count_tks(&line);
	line.tks = tokenizer(&line);
	return (0);
}


/* void	facade()
{

}

void	while_prompt()
{
	char	*line;

	line = readline("miau>");
} */