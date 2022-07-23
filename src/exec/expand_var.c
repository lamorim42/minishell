/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_var.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/22 15:46:38 by lamorim           #+#    #+#             */
/*   Updated: 2022/07/23 09:39:21 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	change_value(t_line *line, t_hash_table *table, unsigned int i);
static char	*join_str_value(char *key, char *value, char *str);
static char	*join_helper(char *new, char *helper);

void	expand_var(t_line *line, t_hash_table *table)
{
	int	i;

	i = 0;
	while (line->lex[i])
	{
		if (ft_strcmp_len(line->lex[i], "VAR") && line->tks[i][0] != '\'')
			while (ft_strchr(line->tks[i], '$'))
				change_value(line, table, i);
		i++;
	}
}

static void	change_value(t_line *line, t_hash_table *table, unsigned int i)
{
	char	*value;
	char	*key;
	char	*temp;

	key = return_var_key(line->tks[i]);
	value = search_item(table, key);
	if (value != NULL)
	{
		temp = line->tks[i];
		line->tks[i] = join_str_value(key, value, line->tks[i]);
		free(temp);
		free (value);
	}
	else
	{
		temp = line->tks[i];
		line->tks[i] = ft_strdup("");
		free(temp);
	}
	free (key);
}

static char	*join_str_value(char *key, char *value, char *str)
{
	unsigned int	i;
	char			*concat_str;
	char			*new_str;

	i = 0;
	concat_str = NULL;
	new_str = NULL;
	if ((ft_strlen(key) + 1) == (ft_strlen(str)))
		return (ft_strdup(value));
	new_str = take_prefix_var(str, &i);
	if (new_str != NULL)
		new_str = join_helper(new_str, value);
	else
		new_str = ft_strdup(value);
	i += (ft_strlen(key) + 1);
	if (str[i])
		concat_str = sufix_var(str, &i);
	if (concat_str != NULL)
	{
		new_str = join_helper(new_str, concat_str);
		free(concat_str);
	}
	return (new_str);
}

static char	*join_helper(char *new, char *helper)
{
	char	*temp;

	temp = new;
	new = ft_strjoin(new, helper);
	free(temp);
	temp = NULL;
	return (new);
}
