/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/22 14:08:59 by lamorim           #+#    #+#             */
/*   Updated: 2022/07/22 16:17:43 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_command(t_pipe_list *node)
{
	return (ft_strncmp(node->args[0], "PIPE", 4)
		&& ft_strncmp(node->args[0], "REDO", 4)
		&& ft_strncmp(node->args[0], "REDA", 4)
		&& ft_strncmp(node->args[0], "REDI", 4)
		&& ft_strncmp(node->args[0], "HERE", 4));
}

char	*take_prefix_var(char *str, unsigned int *i)
{
	char	*sufix;

	sufix = NULL;
	while (str[*i] && str[*i] != '$')
	{
		(*i)++;
	}
	if (*i > 0)
		sufix = ft_substr(str, 0, *i);
	return (sufix);
}

char	*sufix_var(char *str, unsigned int *i)
{
	char	*temp;
	int		len;

	temp = &str[*i];
	len = ft_strlen(temp);
	if (len > 0)
		return (ft_substr(str, *i, len));
	return (NULL);
}
