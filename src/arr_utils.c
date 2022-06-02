/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arr_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 21:04:45 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/01 21:04:55 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**copy_array(char **tokens, int size)
{
	char	**array_cpy;
	int		len;
	int		i;

	len = ft_array_len(tokens);
	array_cpy = (char **)malloc(sizeof(char *) * (len + 1));
	if (array_cpy == NULL)
		printf("!ERROR!\n");
	i = 0;
	while (i < size)
	{
		array_cpy[i] = ft_strdup(tokens[i]);
		i++;
	}
	array_cpy[i] = NULL;
	return (array_cpy);
}
