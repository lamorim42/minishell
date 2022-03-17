/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 09:46:03 by lamorim           #+#    #+#             */
/*   Updated: 2022/03/17 09:46:51 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	new_node(char *str)
{
	int *ptr;

	ptr = (char *)malloc(sizeof(char));

	if (!ptr)
		return (NULL);


// miau 'kit cat' | miau
// node -> "miau\0"
}
