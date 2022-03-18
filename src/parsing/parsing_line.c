/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/18 09:24:14 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/18 09:29:04 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

void	parse(char *line)
{
	size_t	i;
	char	*word;

	i = 0;
	word = split_line(line);
	//printf("word: %s\n", word);
	while (i < strlen(line))
	{
		if (strchr(SEP, line[i]))
		{
			printf("oiii\n");
		}
		i++;
	}
}
