/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 08:29:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/21 20:38:32 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**table_cmds(char **tokens, int size);

int	main(void)
{
	char	*line;
	char	*temp;
	char	**tokens;
	char	**table;
	int		count_sep;
	//int		i;

	count_sep = 0;
	//i = 0;
	line = readline("miau> ");
	temp = line;

	while (*temp)
	{
		if (*temp == '|')
			count_sep++;
		temp++;
	}

	tokens = token(line);
	table = table_cmds(tokens, count_sep);

	free(line);

	return (0);
}

char	**table_cmds(char **tokens, int size)
{
	char	**temp;
	char	**table;
	int		i;

	i = 0;
	temp = tokens;
	table = malloc(sizeof(char *) * (size + 2));
	while (*temp)
	{
		printf("token: %s\n", *temp);
		if (ft_strchr(*temp, '|'))
		{
			temp++;
		}
		else
		{
			printf("else do |\n");
			table[i] = ft_strdup(*temp);
			printf("table[%d] = %s\n", i, table[i]);
			i++;
			temp++;
		}
	}
	table[i] = NULL;
	temp = table;
	while (*temp)
	{
		printf("temp: %s\n", *temp);
		temp++;
	}

	return (table);
}

void	return_error(void)
{
	printf("error!\n");
}
