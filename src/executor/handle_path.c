/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_path.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 09:57:33 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/06 09:28:35 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

char	*get_path(t_line *line)
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
		free(path);
		path = NULL;
	}
	return (path);
}

void	split_path(t_line *line)
{
	char	*path_cmd;

	path_cmd = get_path(line);
	line->path = ft_split(path_cmd, ':');
	free(path_cmd);
}

char	**check_path(t_line *line)
{
	int	i;
	int	j;
	int	len;
	char	*cmd;
	char	**bin;

	j = 0;
	len = ft_array_len(line->cmds_table);
	bin = malloc(sizeof(char *) * (len + 1));
	if(bin == NULL)
		return_error();
	while(line->cmds_table[j])
	{
		i = 0;
		cmd = ft_strjoin("/", line->cmds_table[j]);
		while (line->path[i])
		{
			bin[j] = ft_strjoin(line->path[i], cmd);
			if (!access(bin[j], F_OK))
			{
				free(cmd);
				cmd = NULL;
				break ;
			}
			else
				i++;
			free(bin[j]);
			bin[j] = NULL;
		}
		if (cmd)
			free(cmd);
		if (bin[j] == NULL)
			bin[j] = ft_strdup("null");
		j++;
	}
	bin[j] = NULL;
	return (bin);
}