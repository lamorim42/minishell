/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 19:17:50 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/06 20:51:55 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_var_env(t_hash_table **table, char *str_key, char *val);
void	cd_builtin(t_pipe_list *node, t_hash_table **table)
{
	char	*str_pwd;
	char	*str_oldpwd;
	char	*temp;

	temp = NULL;
	str_oldpwd =  NULL;
	str_pwd = ft_strdup(node->args[1]);
	if (ft_strlen(node->args[0]) > 2)
		error_msg(node->args[0], ": command not found\n");

	if (chdir(str_pwd) == -1)
		error_msg(node->args[1], ":No such file or directory\n");
	else
	{
		//str_oldpwd = search_item((*table), "PWD");
		if (!ft_strncmp(str_pwd, "/", 1))
		{
			printf("Dentro do absoluto\n");
			update_var_env(table, "PWD", str_pwd);
		}
		else
		{
			printf("OIIIII relativo\n");
			temp = ft_strjoin("/", str_pwd);
			str_oldpwd = str_pwd;
			str_pwd = ft_strjoin(str_pwd, temp);
			update_var_env(table, "PWD", str_pwd);
			update_var_env(table, "OLDPWD", str_oldpwd);
			free(temp);
			free(str_oldpwd);
			free(str_pwd);
		}
	}
}

static void	update_var_env(t_hash_table **table, char *str_key, char *val)
{
	printf("Dentro da update\n");
	table_delete(table, str_key);
	hash_insert(table, str_key, val);
}