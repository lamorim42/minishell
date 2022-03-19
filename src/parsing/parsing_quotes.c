/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_quotes.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/18 09:26:45 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/19 19:50:35 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"


char	*handle_quotes(char **ptr_len, char quot)
{
	int	i;
	char	*content;

	i = 0;
	content = NULL;
	if (has_double_quotation((*ptr_len), **ptr_len) == 1)
	{
		(*ptr_len)++;
		while ((*ptr_len)[i])
		{
			if ((*ptr_len)[i] == quot)
			{
				content = worddup((*ptr_len), i);
				(*ptr_len) += i;
				break ;
			}
			i++;
		}
	}
	else
		return_error();
	return (content);
}

// "main function"

int	has_double_quotation(char *str, char quot)
{
	int	ret;
	int	count;

	ret = 0;
	count = 0;
	while (*str)
	{
		if (*str == quot)
			count++;
		str++;
	}
	if (count % 2 == 0)
		ret = 1;
	return (ret);
}