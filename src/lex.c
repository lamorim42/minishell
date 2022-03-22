/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lex.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 16:03:24 by lamorim           #+#    #+#             */
/*   Updated: 2022/03/22 17:15:02 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_array_len(char **array);

char	**tokens_classification(char **tokens)
{
	char	**lex_tokens;
	char	**temp;

	lex_tokens = malloc(sizeof(char *) * (ft_array_len(tokens) + 1));
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
		printf("lex: %s\n", *temp);
		temp++;
	}
	return (lex_tokens);
}

int	ft_array_len(char **array)
{
	int	len;

	len = 0;
	while (*array)
	{
		len++;
		array++;
	}
	return (len);
}