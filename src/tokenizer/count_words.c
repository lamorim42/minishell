/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_words.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 19:43:15 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/24 17:50:46 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

// ta errada!
size_t	count_words(char *s)
{
	size_t	words;
	int		i;

	i = 0;
	words = 0;
	while (s[i])
	{
		if (ft_strchr(" |", s[i]) || s[i] == '\0')
			words++;
		i++;
	}
	printf("%zu\n", words);
	return (words);
}
