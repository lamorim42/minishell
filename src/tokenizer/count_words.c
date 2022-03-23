/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_words.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 19:43:15 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/23 16:18:34 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

size_t	count_words(char *s)
{
	int		is_word;
	size_t	words;

	words = 0;
	is_word = 0;
	while (*s)
	{
		if (ft_strchr("|", *s))
			words++;
		if (!is_word && !ft_strchr("| ", *s))
		{
			is_word = 1;
			words++;
		}
		else if (is_word && ft_strchr("| ", *s))
			is_word = 0;
		s++;
	}
	return (words);
}
