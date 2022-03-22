/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 08:29:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/22 08:38:18 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**table_cmds(char **tokens, int size);
char	**table_red(char **tokens, int size);

int	main(void)
{
	char	*line;
	char	*temp;
	char	**tokens;
	char	**table;
	char	**table_redirect;
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
	table_redirect = table_red(tokens, count_sep);

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
	if (size == 0)
	{
		table = tokens;
		return (table);
	}
	table = malloc(sizeof(char *) * (size + 2));
	while (*temp)
	{
		if (ft_strchr(*temp, '|'))
		{
			temp++;
		}
		else
		{
			table[i] = ft_strdup(*temp);
			i++;
			temp++;
		}
	}
	table[i] = NULL;
	/* temp = table;
	while (*temp)
	{
		printf("temp: %s\n", *temp);
		temp++;
	} */
	return (table);
}

char	**table_red(char **tokens, int size)
{
	char	**temp;
	char	**array_red;
	int		i;

	temp = tokens;
	i = 0;

	array_red = malloc(sizeof(char *) * (size + 1));
	while(*temp)
	{
		if (ft_strchr(*temp, '|') || ft_strchr(*temp, '>'))
		{
			printf("%d\n", i);
			array_red[i] = ft_strdup(*temp);
			i++;
			temp++;
		} else
			temp++;
	}
	array_red[i] = NULL;
	temp = array_red;
	while (*temp)
	{
		printf("array_red: %s\n", *temp);
		temp++;
	}
	return (array_red);
}

void	return_error(void)
{
	printf("error!\n");
}
