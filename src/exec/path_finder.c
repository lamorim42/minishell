/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_finder.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/23 10:12:02 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 17:19:52 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*get_path(t_hash_table **table)
{
	int		i;
	char	*path;

	i = 0;
	path = NULL;
	path = search_item(*table, "PATH");
	return (path);
}

static char	**split_path(t_hash_table **table)
{
	char	*path_cmd;
	char	**path;

	path = NULL;
	path_cmd = get_path(table);
	if (path_cmd != NULL)
		path = ft_split(path_cmd, ':');
	free (path_cmd);
	return (path);
}

static char	*access_path(char *cmd, char **paths)
{
	char	*cmd_bin;
	int		i;
	char	*bin;

	i = 0;
	bin = NULL;
	cmd_bin = ft_strjoin("/", cmd);
	while (paths[i])
	{
		bin = ft_strjoin(paths[i], cmd_bin);
		if (!access(bin, F_OK))
			break ;
		else
		{
			free(bin);
			bin = NULL;
		}
		i++;
	}
	free(cmd_bin);
	ft_free_arr(paths);
	return (bin);
}

char	*path_finder(char *cmd, t_hash_table **table)
{
	char	**paths;
	char	*bin;

	bin = NULL;
	paths = split_path(table);
	if (paths != NULL)
		bin = access_path(cmd, paths);
	return (bin);
}
