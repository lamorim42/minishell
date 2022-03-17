/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 08:29:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/17 10:25:30 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	parse(char *line);
char	**split_line(char *line);

int	main(void)
{
	char *line;

	printf("miau>");
	line = readline(STDIN);
	split_line(line);

	parse(line);
	free(line);

	return (0);
}

char	**split_line(char *line)
{
	char	**splited;

	splited = ft_split(line, ' ');
	printf("splited: %s\n", splited[0]);
	printf("splited: %s\n", splited[1]);
	printf("splited: %s\n", splited[2]);
	return (splited);
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
