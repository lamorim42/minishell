/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 09:57:33 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/30 11:34:55 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

/* char	*get_path(t_line *line)
{
	int		i;
	char	*path;

	i = 0;
	path = NULL;
	while (line->envp[i])
	{
		if (ft_strncmp(line->envp[i], "PATH=", 5) == 0)
		{
			path = ft_strdup(line->envp[i] + 5);
			break ;
		}
		i++;
	}
	//printf("%s\n", line->path);
	return (path);
}

void	split_path(t_line *line)
{
	char	*path_cmd;

	path_cmd = get_path(line);
	line->path = ft_split(path_cmd, ':');

	print_array("print array", line->path);
}

char	*check_path(t_line *line)
{
	int	i;
	char	*cmd;
	char	*bin;
	//char	*temp;

	i = 0;

	while(line->path[i])
	{
		cmd = ft_strjoin("/", line->array_cmds[i]);
		bin = ft_strjoin(line->path[i], cmd);
		if (access(bin, F_OK | X_OK) == 1)
		{
			free(bin);
			break ;
		}
		else
			return (bin);
		i++;
	}
	return (NULL);
} */