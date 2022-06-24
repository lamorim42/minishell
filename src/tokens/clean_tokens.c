/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/21 17:55:50 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/24 19:04:15 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*clean_quots(char *tks);
static int	count_quots(char *tks);
static char	*clean_dolar_and_get_var(char *tks, t_hash_table *table);

char	**clean_tokens(t_line *line, t_hash_table *table)
{
	char	**cmds;
	int		i;

	i = 0;
	cmds = NULL;
	cmds = (char **)malloc(sizeof(char **) * (line->tks_nbr + 1));
	while (line->tks[i])
	{
		if (ft_strchr(line->tks[i], '\"'))
			cmds[i] = clean_quots(line->tks[i]);
		else if (ft_strchr(line->tks[i], '$'))
			cmds[i] = clean_dolar_and_get_var(line->tks[i], table);
		else
			cmds[i] = ft_strdup(line->tks[i]);
		i++;
	}
	cmds[i] = NULL;
	return (cmds);
}

static char	*clean_dolar_and_get_var(char *tks, t_hash_table *table)
{
	char	*var;
	char	*temp;

	var = NULL;
	if (tks[0] == '$')
	{
		tks = ft_strtrim(tks, "$");
		temp = search_item(&table, tks);
		free (tks);
		if (temp != NULL)
			var = ft_strdup(temp);
		else
			var = ft_strdup("");
		return (var);
	}
	return tks;
}

static char	*clean_quots(char *tks)
{
	char	*copy;
	int		i;
	int		j;
	int		quots;

	quots = 0;
	i = 0;
	j = 0;
	if (tks[0] == '\"' && tks[ft_strlen(tks) - 1] == '\"')
		copy = ft_strtrim(tks, "\"");
	else
	{
		quots = count_quots(tks);
		copy = (char *)malloc(sizeof(char *) * (ft_strlen(tks) - quots + 1));
		while (tks[i] != '\0')
		{
			if (tks[i] != '\"')
				copy[j++] = tks[i++];
			else
				i++;
		}
		copy[j] = '\0';
	}
	return (copy);
}

static int	count_quots(char *tks)
{
	int	quots;
	int	i;

	quots = 0;
	i = 0;
	while (tks[i] != '\0')
	{
		if (tks[i] == '\"')
			quots++;
		i++;
	}
	return (quots);
}
