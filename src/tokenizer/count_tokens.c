/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 19:43:15 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/25 16:11:54 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

void	count_word(char **s, char quot, size_t *words);

// ta errada!
size_t	count_tokens(char *s)
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
	printf("%zu\n", words);
	return (words);
}

void	count_word(char **s, char quot, size_t *words)
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