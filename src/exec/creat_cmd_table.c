/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creat_cmd_table.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/02 20:10:01 by lamorim           #+#    #+#             */
/*   Updated: 2022/07/22 16:14:28 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**make_args(t_line *line, int *index);
int		get_arg_size(t_line *line, int index);

void	creat_list_cmd(t_line *line)
{
	int	i;

	i = 0;
	while (line->tks[i])
		add_back_list(&(line->list_cmds), new_node(make_args(line, &i)));
}

char	**make_args(t_line *line, int *index)
{
	char	**arg;
	int		size;

	size = get_arg_size(line, *index);
	arg = copy_array(&(line->ctks[*index]), size);
	if (ft_strncmp(line->lex[*index], "WORD", 4)
		&& ft_strncmp(line->lex[*index], "VAR", 3))
	{
		free(arg[0]);
		arg[0] = ft_strdup(line->lex[*index]);
	}
	*index += size;
	return (arg);
}

int	get_arg_size(t_line *line, int index)
{
	int		size;

	size = 0;
	if (!ft_strncmp(line->lex[index], "WORD", 4)
		|| !ft_strncmp(line->lex[index], "VAR", 3))
	{
		while (line->lex[index] && (!ft_strncmp(line->lex[index], "WORD", 4)
				|| !ft_strncmp(line->lex[index], "VAR", 3)))
		{
			index++;
			size++;
		}
	}
	else
	{
		if (!ft_strncmp(line->lex[index], "PIPE", 4))
			size = 1;
		else
			size = 2;
	}
	return (size);
}

char	*char_cat(char *str, char c)
{
	char	*temp;
	char	*aux;

	temp = malloc(sizeof(char) * 2);
	temp[0] = c;
	temp[1] = '\0';
	aux = ft_strjoin(str, temp);
	free(temp);
	return (aux);
}
