/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sintax_analysis.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/21 16:36:27 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/20 18:50:32 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	find_pair(char *str, int *i, char quot);

int	sintax_analysis(char **lex)
{
	int	i;

	i = 0;
	if (*lex && !ft_strncmp(lex[0], "PIPE", 4))
		return (FALSE);
	while (lex[i])
	{
		if (!ft_strncmp(lex[i], "PIPE", 4)
			&& (lex[i + 1] != NULL && !ft_strncmp(lex[i + 1], "PIPE", 4)))
			return (FALSE);
		if (!ft_strncmp(lex[i], "REDO", 4)
			&& (lex[i + 1] == NULL || ft_strncmp(lex[i + 1], "WORD", 4)))
			return (FALSE);
		if (!ft_strncmp(lex[i], "REDA", 4)
			&& (lex[i + 1] == NULL || ft_strncmp(lex[i + 1], "WORD", 4)))
			return(FALSE);
		if (!ft_strncmp(lex[i], "REDI", 4)
			&& (lex[i + 1] == NULL || ft_strncmp(lex[i + 1], "WORD", 4)))
			return(FALSE);
		if (!ft_strncmp(lex[i], "HERE", 4)
			&& (lex[i + 1] == NULL || ft_strncmp(lex[i + 1], "WORD", 4)))
			return(FALSE);
		i++;
	}
	return (TRUE);
}

int	verify_quots(char *str)
{
	int	i;
	int	flag;

	i = 0;
	flag = TRUE;
	while (str[i])
	{
		if (str[i] == '\"')
			flag = find_pair(str, &i, '\"');
		else if (str[i] == '\'')
			flag = find_pair(str, &i, '\'');
		else
			i++;
		if (!flag)
			break ;
	}
	return (flag);
}

static int	find_pair(char *str, int *i, char quot)
{
	int	find;

	find = FALSE;
	(*i)++;
	while (str[*i] && str[*i] != quot)
		(*i)++;
	if (str[*i] == quot)
	{
		(*i)++;
		find = TRUE;
	}
	return (find);
}
