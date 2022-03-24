/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexical_analysis.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 16:03:24 by lamorim           #+#    #+#             */
/*   Updated: 2022/03/24 09:53:10 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**lexical_analysis(char **tokens)
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
	print_array("Lex", lex_tokens);
	return (lex_tokens);
}
