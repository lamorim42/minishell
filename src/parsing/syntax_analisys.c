/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_analisys.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 08:34:54 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/31 21:52:56 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

static void	syntax_error(void);

void	syntax_analisys(char	**lex_tokens)
{
	int	i;
	char	**temp;
	temp = lex_tokens;

	i = 1;
	if (!ft_strncmp(temp[0], "PIPE", 4))
		syntax_error();
	while (temp[i])
	{
		if (!ft_strncmp(temp[i], "PIPE", 4)
		&& !ft_strncmp(temp[i - 1], temp[i], 4))
			syntax_error();
		i++;
	}
	if (!ft_strncmp(temp[i - 1], "PIPE", 4))
	{
		printf("Lembrar de lidar com o | no final aqui\n");
	}
}

static void	syntax_error(void)
{
	printf("syntax error near unexpected token\n");
	exit(1);
}
