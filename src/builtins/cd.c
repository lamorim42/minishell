/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 19:17:50 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 18:21:48 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern struct s_minishell	g_minishell;

static void	cd_expand(t_hash_table **table, char *old_pwd, char *expand);

void	cd_builtin(t_pipe_list *node, t_hash_table **table)
{
	char	*old_pwd;
	char	buffer[500];

	getcwd(buffer, 500);
	old_pwd = ft_strdup(buffer);
	if (node->args[1] == NULL || ft_strcmp_len(node->args[1], "~"))
		cd_expand(table, old_pwd, "HOME");
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

void	update_var_env(t_hash_table **table, char *str_key, char *val)
{
	char	*valid;

	valid = search_item(*table, str_key);
	if (valid != NULL)
		table_delete(table, str_key);
	hash_insert(table, str_key, val);
	free(valid);
}

static void	cd_expand(t_hash_table **table, char *old_pwd, char *expand)
{
	char	*aux;

	aux = search_item(*table, expand);
	if (aux != NULL)
	{
		if (chdir(aux) == -1)
		{
			g_minishell.line->status_code = 1;
			perror(aux);
			free(aux);
			return ;
		}
		update_var_env(table, "PWD", aux);
		update_var_env(table, "OLDPWD", old_pwd);
	}
	else if (aux == NULL && ft_strcmp_len(expand, "HOME"))
		error_msg("cd: ", "HOME not set\n");
	free(aux);
}
