/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 08:29:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/17 09:18:34 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	parse(char *line);

int	main(void)
{
	char *line;

	printf("miau>");
	line = readline(STDIN);

	linked(line);

	parse(line);
	free(line);

	return (0);
}

void	new_node(char *str)
{
	int *ptr;

	ptr = (char *)malloc(sizeof(char));

	if (!ptr)
		return (NULL);


// miau 'kit cat' | miau
// node -> "miau\0"
}

void	linked(char *line)
{
	t_stack *
}

void	parse(char *line)
{
	int	i;

	i = 0;
	while ((size_t)i < strlen(line))
	{
		if (strchr(SEP, line[i]))
		{
			printf("oiii\n");
		}
		i++;
	}
}
