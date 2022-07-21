/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/21 17:55:50 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/21 16:44:05 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*clean_quots(char *tks);
static char	*clean_in_middle(char *tks);

char	**clean_tokens(t_line *line)
{
	char	**cmds;
	int		i;

	i = 0;
	cmds = NULL;
	cmds = (char **)malloc(sizeof(char **) * (line->tks_nbr + 1));
	while (line->tks[i])
	{
		if (ft_strchr(line->tks[i], '\"') || ft_strchr(line->tks[i], '\''))
			cmds[i] = clean_quots(line->tks[i]);
		else
			cmds[i] = ft_strdup(line->tks[i]);
		i++;
	}
	cmds[i] = NULL;
	return (cmds);
}

static char	*clean_quots(char *tks)
{
	char	*copy;

	if (tks[0] == '\"' && tks[ft_strlen(tks) - 1] == '\"')
		copy = ft_strtrim(tks, "\"");
	else if (tks[0] == '\'' && tks[ft_strlen(tks) - 1] == '\'')
		copy = ft_strtrim(tks, "\'");
	else
		copy = clean_in_middle(tks);
	return (copy);
}

int	count_quots(char *tks)
{
	int	quots;
	int	i;

	quots = 0;
	i = 0;
	while (tks[i] != '\0')
	{
		if (tks[i] == '\"' || tks[i] == '\'')
			quots++;
		i++;
	}
	return (quots);
}

static char	*clean_in_middle(char *tks)
{
	int		quots;
	char	*copy;
	int		i;
	int		j;

	i = 0;
	j = 0;
	quots = count_quots(tks);
	copy = (char *)malloc(sizeof(char *) * (ft_strlen(tks) - quots + 1));
	while (tks[i] != '\0')
	{
		if (tks[i] != '\"' && tks[i] != '\'')
			copy[j++] = tks[i++];
		else
			i++;
	}
	copy[j] = '\0';
	return (copy);
}
