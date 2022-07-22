/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 19:17:50 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/21 20:43:48 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_var_env(t_hash_table **table, char *str_key, char *val);
static void	cd_expand(t_hash_table **table, char *old_pwd, char *expand);

void	cd_builtin(t_pipe_list *node, t_hash_table **table)
{
	char	*old_pwd;
	char	buffer[500];

	getcwd(buffer, 500);
	old_pwd = ft_strdup(buffer);
	if (node->args[1] == NULL || ft_strcmp_len(node->args[1], "~"))
		cd_expand(table, old_pwd, "HOME");
	else if (ft_strcmp_len(node->args[1], "-"))
		cd_expand(table, old_pwd, "OLDPWD");
	else if (chdir(node->args[1]) == -1)
	{
		g_minishell.line->status_code = 1;
		perror(node->args[1]);
	}
	else
	{
		getcwd(buffer, 500);
		update_var_env(table, "PWD", buffer);
		update_var_env(table, "OLDPWD", old_pwd);
	}
	free(old_pwd);
}

static void	update_var_env(t_hash_table **table, char *str_key, char *val)
{
	table_delete(table, str_key);
	hash_insert(table, str_key, val);
}

static void	cd_expand(t_hash_table **table, char *old_pwd, char *expand)
{
	char	*aux;

	aux = search_item(*table, expand);
	if (chdir(aux) == -1)
	{
		g_minishell.line->status_code = 1;
		perror(aux);
		free(aux);
		return ;
	}
	update_var_env(table, "PWD", aux);
	update_var_env(table, "OLDPWD", old_pwd);
	free(aux);
}
