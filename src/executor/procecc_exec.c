/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   procecc_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 16:39:22 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/29 11:23:39 by dmonteir         ###   ########.fr       */
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
	char	**temp;

	i = 0;
	j = 0;
	temp = line->tks;
	/* while (temp[i])
	{
		printf("%s\n", temp[i++]);
	} */
	//printf("%s\n", temp[0]);

	line->array_cmds = malloc(sizeof(char **) * ((line->nb_pipes * 2) + 2));
	while (temp[i])
	{
		if (temp[i + 1] == NULL)
		{
			line->array_cmds[j] = ft_strdup(temp[i]);
			j++;
		}
		else if (temp[i + 1] && temp[i + 1][0] == '|')
		{
			line->array_cmds[j] = ft_strdup(temp[i]);
			j++;
		}
		i++;
	}
	line->array_cmds[j] = NULL;
	j = 0;
	while (line->array_cmds[j])
	{
		printf("%s\n", line->array_cmds[j++]);
	}
}
