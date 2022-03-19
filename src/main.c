/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 08:29:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/19 18:27:04 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"



int	main(void)
{
	t_stack	*stack;
	char *line;

	line = readline("miau> ");

	stack = malloc(sizeof(t_stack));
	//create_stack(&stack);
	parse(line, &stack);
	free(line);

	return (0);
}

void	return_error(void)
{
	perror("Error!");
	exit(1);
}
