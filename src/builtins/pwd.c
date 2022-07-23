/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 07:38:27 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 16:17:23 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	pwd_builtin(t_hash_table **table)
{
	char	*str_pwd;

	str_pwd = search_item(*table, "PWD");
	if (str_pwd != NULL)
		printf("%s\n", str_pwd);
	free(str_pwd);
}
