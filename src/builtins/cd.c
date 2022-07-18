/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 19:17:50 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/17 19:40:08 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_var_env(t_hash_table **table, char *str_key, char *val);
void	cd_builtin(t_pipe_list *node, t_hash_table **table)
{
	char	*str_oldpwd;
	char	buffer[500];
	char	*home;

	getcwd(buffer, 500);
	str_oldpwd = ft_strdup(buffer);
	if (ft_strlen(node->args[0]) > 2)
	{
		error_msg(node->args[0], ": command not found\n");
		return ;
	}
	if (node->args[1] == NULL)
	{
		home = search_item(*table, "HOME");
		chdir(home);
		update_var_env(table, "PWD", home);
		update_var_env(table, "OLDPWD", str_oldpwd);
		free(home);
	}
	else if (chdir(node->args[1]) == -1)
	{
		error_msg(node->args[1], ": No such file or directory\n");
		return ;
	}
	else
	{
		getcwd(buffer, 500);
		update_var_env(table, "PWD", buffer);
		update_var_env(table, "OLDPWD", str_oldpwd);
	}
	free(str_oldpwd);
}

static void	update_var_env(t_hash_table **table, char *str_key, char *val)
{
	table_delete(table, str_key);
	hash_insert(table, str_key, val);
}