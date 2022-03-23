/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_words.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 19:43:15 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/23 08:14:09 by dmonteir         ###   ########.fr       */
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
		else if (!is_word && !ft_strchr(" ", *s))
		{
			is_word = 1;
			words++;
		}
		else if (is_word && ft_strchr(" ", *s))
			is_word = 0;
		s++;
	}
	return (words);
}
