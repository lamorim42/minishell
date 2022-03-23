/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gramatic.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 08:34:54 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/23 10:59:11 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

void	rules_grammar(char	**lex_tokens)
{
	int	i;
	char	**temp;
	temp = lex_tokens;

	i = 0;
	if (ft_strncmp(temp[0], "WORD", 4))
		syntax_error();

	while (temp[i])
		i++;
	if (ft_strncmp(temp[i - 1], "|", 1))
	{
		printf("LEmbrar de lidar com o | no final aqui\n");
	}
}

void	syntax_error(void)
{
	printf("syntax error near unexpected token\n");
	exit(1);
}
