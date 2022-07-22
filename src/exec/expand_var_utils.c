/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_var_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/22 18:03:49 by lamorim           #+#    #+#             */
/*   Updated: 2022/07/22 18:04:19 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*return_var_key(char *str)
{
	int		i;
	int		len;
	int		cpy;
	char	*key;

	i = 0;
	len = 0;
	cpy = 0;
	key = NULL;
	while (str[i])
	{
		if (str[i] == '$')
		{
			cpy = i + 1;
			key_len(str, &i, &len);
			len--;
			break ;
		}
		else
			i++;
	}
	key = ft_substr(str, (unsigned int)cpy, ((size_t)len));
	return (key);
}

void	key_len(char *str, int *i, int *len)
{
	while (str[*i] && !ft_strchr("\"\',:./[]{}=-+*!@# ", str[*i]))
	{
		(*len)++;
		(*i)++;
	}
}
