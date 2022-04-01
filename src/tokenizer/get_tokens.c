/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_tokens.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 19:44:20 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/31 21:47:18 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

static void		count_word(char **s, char quot, size_t *words);
static size_t	count_tokens(char *s);

char	**get_tokens(char *line)
{
	char	**tokens;
	size_t	words;
	char	*ptr;
	int		i;

	if (!line)
		return (NULL);
	words = count_tokens(line);
	tokens = malloc(sizeof(char *) * (words + 1));
	i = 0;
	ptr = line;
	while (*ptr)
	{
		if (*ptr != SPC && *ptr != QUOT && *ptr != DQUOT && *ptr != '|')
		{
			tokens[i] = worddup(&ptr);
			i++;
		}
		else if (*ptr == SPC)
			ptr++;
		else if (*ptr == QUOT || *ptr == DQUOT)
		{
			tokens[i] = quotdup(&ptr, *ptr);
			i++;
		}
		else if (*ptr == '|')
		{
			tokens[i] = ft_strdup("|");
			i++;
			ptr++;
		}
	}
	tokens[i] = NULL;
	return (tokens);
}

static size_t	count_tokens(char *s)
{
	size_t	words;
	int		is_word;

	is_word = 0;
	words = 0;

	while (*s)
	{
		if (ft_strchr("|", *s))
			words++;
		if (*s == QUOT || *s == DQUOT)
		{
			if (has_double_quotation(s, *s) == 1)
				{
					printf("Don't has dquot!\n");
					exit(1);
				}
			count_word(&s, *s, &words);
		}
		else if (!is_word && !ft_strchr("| ", *s))
		{
			is_word = 1;
			words++;
		}
		else if (is_word && ft_strchr("| ", *s))
			is_word = 0;
		s++;
	}
	//printf("%zu\n", words);
	return (words);
}

static void	count_word(char **s, char quot, size_t *words)
{
	int	i;

	i = 1;
	while ((*s)[i] != quot && (*s)[i])
	{
		if ((*s)[i] == SPC)
		{
			(*words)++;
			break;
		}
		i++;
	}
	if ((*s)[i] != quot)
	{
		while ((*s)[i] != quot && (*s)[i])
			i++;
	}
	else if (ft_strchr("| ", (*s)[(i + 1)]))
		(*words)++;
	*s += i;
}

// Não verificar usando while, mas com uma strchr para encontrar " " ou qualquer
//outro separador
//a verificação com else if conta uma palavra quando encontra "|" mesmo quando
//  ca"t"|" cat"|, preciso verificar melhor