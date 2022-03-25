/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   procecc_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 16:39:22 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/25 19:38:32 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

char	**copy_array(char **tokens, int size)
{
	char	**array_cpy;
	int		len;
	int		i;


	len = ft_array_len(tokens);
	array_cpy = malloc(sizeof(char *) * (len + 1));

	i = 0;
	while (i < size)
	{
		array_cpy[i] = tokens[i];
		i++;
	}
	array_cpy[i] = NULL;
	return(array_cpy);
}

void	count_pipe(t_line *line)
{
	char	*temp;

	temp = line->pipeline;
	while (*temp)
	{
		if (*temp == '|')
			line->nb_pipes++;
		temp++;
	}
}

void	create_cmd(t_line *line)
{
	int		i;


	i = 0;
	while(line->tks[i])
	{
		if (ft_strchr(line->tks[i], '|'))
			break;
		line->nb_cmds++;
		i++;
	}
}

void	array_cmd(t_line *line)
{
	int		i;
	int		j;
	int		reset;
	char	**temp;

	i = 0;
	j = 0;
	temp = line->tks;

	line->array_cmds = malloc(sizeof(char **) * (line->nb_pipes + 2));
	while (temp[i])
	{
		if (ft_strchr(temp[i], '|'))
		{
			if (j == 0)
			{
				line->array_cmds[j] = copy_array(&temp[j], reset);
				reset = 0;
				j++;
			}
		} else if (ft_strchr(temp[i + 1], '\0'))
		{
			line->array_cmds[j] = copy_array(&temp[i], reset);
			reset = 0;
			j++;
		}
		i++;
		reset++;
	}
	line->array_cmds[j] = NULL;
	print_array("array_cmd", line->array_cmds[0]);
	//print_array("array_cmd", line->array_cmds[1]);
}
