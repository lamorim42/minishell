/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 19:17:50 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/22 20:42:23 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_var_env(t_hash_table **table, char *str_key, char *val);
void	cd_builtin(t_pipe_list *node, t_hash_table **table)
{
	char	*str_pwd;
	char	*str_oldpwd;

	str_pwd = NULL;
	str_oldpwd =  NULL;
	str_pwd = node->args[1];
	if (ft_strlen(node->args[0]) > 2)
	{
		printf("%s: command not found\n", node->args[0]);
		//exit(127);
	}
	if (ft_strncmp(str_pwd, "~", 1) == 0)
		str_pwd = search_item(table, "HOME");
	if (chdir(str_pwd) == -1)
	{
		printf("bash: cd: %s: No such file or directory\n", node->args[1]);
	}
	else
	{
		str_oldpwd = search_item(table, "PWD");
		if (ft_strncmp(str_pwd, "/home", 5) != 0)
		{
			str_oldpwd = ft_strjoin(str_oldpwd, "/");
			str_pwd = ft_strjoin(str_oldpwd, str_pwd);
			update_var_env(table, "PWD", str_pwd);
		}
		else
		{
			update_var_env(table, "OLDPWD", str_pwd);
			update_var_env(table, "PWD", str_pwd);
		}
	}
}

static void	update_var_env(t_hash_table **table, char *str_key, char *val)
{
	table_delete(table, str_key);
	hash_insert(table,str_key, val);
}