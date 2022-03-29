/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   procecc_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 16:39:22 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/29 17:10:24 by dmonteir         ###   ########.fr       */
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
	int		len;
	int		size;

	len = 0;
	size = 0;

	line->array_cmds_cpy = malloc(sizeof(char **) * (line->nb_pipes * 2) + 2);


	//line->array_cmds_cpy[1][0] = ft_strdup(line->tks[len]);

	while (line->tks[len])
	{
		while (ft_strncmp(line->lex[len], "PIPE", 4))
			len++;
		if (size == 0)
			line->array_cmds_cpy[size] = copy_array(line->tks, len);
		else
			line->array_cmds_cpy[size + 1] = copy_array(&line->tks[len], 1);
		size++;
		len++;
	}
	print_array("array\n", line->array_cmds_cpy[0]);
	print_array("array\n", line->array_cmds_cpy[1]);
	//printf("%s\n", line->array_cmds_cpy[1]);
}

//precisamos de um array que conte como 1 todos cmds antes e depois dos separadores.
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

	line->array_cmds = malloc(sizeof(char **) * ((line->nb_pipes * 2) + 1));
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
	/* while (line->array_cmds[j])
	{
		printf("%s\n", line->array_cmds[j++]);
	} */
}
