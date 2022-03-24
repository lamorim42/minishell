/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_words.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 19:43:15 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/24 18:46:47 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

// ta errada!
size_t	count_words(char *s)
{
	size_t	words;
	int		is_word;

	is_word = 0;
	words = 0;

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
	printf("%zu\n", words);
	return (words);
}
