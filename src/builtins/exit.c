/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:09:53 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/18 21:26:58 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_char_number(char *str, int *status_code)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (ft_isdigit(str[i]) == 0)
		{
			printf("%s is not a number!\n", str);
			*status_code = 2;
			break ;
		}
		i++;
	}
	return (*status_code);
}

void	exit_builtin(t_pipe_list *node, t_hash_table **table)
{
	int	i;
	int	status_code;

	i = 1;
	status_code = 0;
	while (node->args[i])
	{
		if (i > 1)
		{
			error_msg(node->args[0], ": too many arguments\n");
			g_minishell.line->status_code = 1;
			return ;
		}
		if (is_char_number(node->args[1], &status_code) == 2)
			break ;
		i++;
	}
	free_line(g_minishell.line);
	free_table(table);
	exit(status_code);
}
