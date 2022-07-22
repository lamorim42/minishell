/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_table.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 12:09:39 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/22 13:50:40 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_split_chr(char *str, char c);
static void	preper_to_insert(char **split);

t_hash_table	*create_table(int size)
{
	t_hash_table	*table;
	int				i;

	i = 0;
	table = (t_hash_table *)malloc(sizeof(t_hash_table));
	table->size = size;
	table->count = 0;
	table->item = (t_hash_item **)ft_calloc(sizeof(t_hash_item *), size);
	while (i < table->size)
	{
		table->item[i] = NULL;
		i++;
	}
	return (table);
}

t_hash_table	*population_hash_table(t_line *line, t_hash_table **table)
{
	int		i;
	char	**var;

	i = 0;
	(*table) = create_table(SIZE_TABLE);
	while (line->envp[i])
	{
		var = ft_split(line->envp[i], '=');
		if (count_split_chr(line->envp[i], '=') > 1)
			preper_to_insert(var);
		if (var[0] != NULL && var[1] != NULL)
			hash_insert(table, var[0], var[1]);
		else if (var[0] != NULL)
			hash_insert(table, var[0], "");
		ft_free_arr(var);
		i++;
	}
	hash_insert(table, "?", "0");
	return (*table);
}

static int	count_split_chr(char *str, char c)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	if (str == NULL || *str == '\0')
		return (count);
	while (str[i])
	{
		if (str[i] == c)
			count++;
		i++;
	}
	return (count);
}

static void	preper_to_insert(char **split)
{
	int		i;
	char	*temp;

	i = 1;
	while (split[i])
	{
		if (split[i + 1] != NULL)
		{
			temp = ft_strdup(split[i]);
			free(split[i]);
			split[i] = ft_strjoin(temp, "=");
			free(temp);
		}
		if (i > 1)
		{
			temp = ft_strdup(split[1]);
			free(split[1]);
			split[1] = ft_strjoin(temp, split[i]);
			free(temp);
		}
		i++;
	}
}
