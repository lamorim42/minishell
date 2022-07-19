/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 07:38:27 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/18 21:42:16 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	pwd_builtin(t_hash_table **table)
{
	char	*str_pwd;

	str_pwd = search_item(*table, "PWD");
	printf("%s\n", str_pwd);
	free(str_pwd);
}
