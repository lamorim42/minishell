/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 16:03:24 by lamorim           #+#    #+#             */
/*   Updated: 2022/03/23 08:23:51 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_array_len(char **array);

char	**tokens_classification(char **tokens)
{
	char	**lex_tokens;
	char	**temp;
	int		len;
	len = ft_array_len(tokens);
	lex_tokens = malloc(sizeof(char *) * (len + 1));
	temp = lex_tokens;
	while (*tokens)
	{
		if (!ft_strncmp(*tokens, "|", 1))
			*temp = ft_strdup("PIPE");
		else
			*temp = ft_strdup("WORD");
		tokens++;
		temp++;
	}
	*temp = NULL;
	temp = lex_tokens;
	while (*temp)
	{
		temp++;
	}
	return (lex_tokens);
}

int	ft_array_len(char **array)
{
	int	len;
	char	**temp;

	temp = array;
	len = 0;
	while (*temp)
	{
		len++;
		temp++;
	}
	return (len);
}