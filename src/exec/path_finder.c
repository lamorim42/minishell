/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_finder.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/23 10:12:02 by dmonteir          #+#    #+#             */
/*   Updated: 2022/06/02 19:26:52 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*get_path(t_line *line)
{
	int		i;
	char	*path;

	i = 0;
	path = NULL;
	while (line->envp[i])
	{
		if (!ft_strncmp(line->envp[i], "PATH=", 5))
		{
			path = ft_strdup(line->envp[i] + 5);
			break ;
		}
		i++;
	}
	return (path);
}

static char	**split_path(t_line *line)
{
	char	*path_cmd;
	char	**path;

	path_cmd = get_path(line);
	path = ft_split(path_cmd, ':');
	free (path_cmd);
	return (path);
}

char	*path_finder(t_line *line, char *cmd)
{
	char	**paths;
	char	*bin;
	char	*cmd_bin;
	int		i;

	paths = split_path(line);
	cmd_bin = ft_strjoin("/", cmd);
	i = 0;
	while (paths[i])
	{
		bin = ft_strjoin(paths[i], cmd_bin);
		if (!access(bin, F_OK))
		{
			free(cmd_bin);
			break ;
		}
		else
		{
			free(bin);
			bin = NULL;
		}
		i++;
	}
	ft_free_arr(paths);
	return (bin);
}
