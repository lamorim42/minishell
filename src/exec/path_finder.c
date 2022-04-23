/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_finder.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/23 10:12:02 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/23 11:55:14 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int ft_array_len(char **array)
{
	int len;

	len = 0;
	while (array[len])
		len++;
	return (len);
}

void	print_array(char *msg, char **array)
{
	int i = 0;
	while (array[i])
	{
		dprintf(2, "%s: %s\n", msg, array[i]);
		i++;
	}
	dprintf(2, "%s\n", array[i]);
}

static char *get_path(t_line *line)
{
	int i;
	char *path;

	i = 0;
	path = NULL;
	while (line->envp[i])
	{
		if (!ft_strncmp(line->envp[i], "PATH=", 5))
		{
			path = ft_strdup(line->envp[i] + 5);
			break;
		}
		i++;
	}
	return (path);
}

static char **split_path(t_line *line)
{
	char *path_cmd;
	char **path;

	path_cmd = get_path(line);
	path = ft_split(path_cmd, ':');
	free(path_cmd);
	return (path);
}

char *path_finder(t_line *line)
{
	char	**paths;
	char	*bin;
	char	*cmd;
	int		len;
	int		i;

	len = ft_array_len(line->cmds);
	paths = split_path(line);
	cmd = ft_strjoin("/", line->cmds[0]);
	i = 0;
	while (paths[i])
	{
		bin = ft_strjoin(paths[i], cmd);
		if (!access(bin, F_OK))
		{
			free(cmd);
			break ;
		}
		else
		{
			free(bin);
			bin = NULL;
		}
		i++;
	}
	printf("bin: %s\n", bin);
	return (bin);
}
