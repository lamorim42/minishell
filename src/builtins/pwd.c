/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 07:38:27 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/17 16:29:33 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	pwd_builtin(t_hash_table **table)
{
	char	*str_pwd;

	str_pwd = search_item(table, "PWD");
	printf("%s\n", str_pwd);
}
