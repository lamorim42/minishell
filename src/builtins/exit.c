/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 18:09:53 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/07 19:27:40 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_char_number(char *str, int status_code)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (ft_isdigit(str[i]) == 0)
		{
			printf("%s is not a number!\n", str);
			status_code = 2;
			break ;
		}
		i++;
	}
	return (status_code);
}

void	exit_builtin(t_line *line, t_pipe_list *node, t_hash_table **table)
{
	int	i;
	int	status_code;

	i = 1;
	status_code = 0;
	if (ft_strlen(node->args[0]) > 4)
	{
		error_msg(node->args[0], ": command not found");
		status_code = 127;
	}
	while (node->args[i])
	{
		if (i > 1)
		{
			error_msg(node->args[0], ": too many arguments");
			status_code = 126;
		}
		if (is_char_number(node->args[1], status_code) == 2)
			break ;
		status_code = ft_atoi(node->args[1]);
		i++;
	}
	free_line(line);
	free_table(table);
	exit(status_code);
}
