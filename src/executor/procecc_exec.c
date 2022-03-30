/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   procecc_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 16:39:22 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/29 23:02:26by lamorim          ###   ########.fr       */
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
		array_cpy[i] = ft_strdup(tokens[i]);
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
	char	**temp_lex = NULL;
	char	**temp_tks = NULL;

	size = 0;
	line->array_cmds = malloc(sizeof(char **) * ((line->nb_pipes * 2) + 2));
	temp_lex = line->lex;
	temp_tks = line->tks;
	while (*temp_tks)
	{
		len = 0;
		while (temp_lex[len] && ft_strncmp(temp_lex[len], "PIPE", 4))
			len++;
		line->array_cmds[size] = copy_array(temp_tks, len);
		size++;
		if (temp_tks[len])
		{
			line->array_cmds[size] = copy_array(&temp_tks[len], 1);
			size++;
		}
		temp_tks += len + (temp_tks[len] != NULL);
		temp_lex += len + (temp_lex[len] != NULL);
	}
	line->array_cmds[size] = NULL;
	/* len = 0;
	while (line->array_cmds[len])
	{
		print_array("array[len]", line->array_cmds[len]);
		len++;
	} */
}


void	str_cmd(t_line *line)
{
	int	size;
	int	len;
	char	*temp;

	size = 0;
	line->array_cmds_cpy = malloc(sizeof(char **) * (line->nb_pipes + 2));
	temp = line->pipeline;
	while (*temp)
	{
		len = 0;
		while (temp[len] && temp[len] != '|')
			len++;
		line->array_cmds_cpy[size] = ft_substr(temp, 0, len);
		size++;
		temp += len + (temp[len] != '\0');
	}
	line->array_cmds_cpy[size] = NULL;

	print_array("array_cpy", line->array_cmds_cpy);

}

void	ft_free_arrcmds(char ***mtx)
{
	int		i;

	i = 0;
	while (mtx[i])
	{
		ft_free_array(mtx[i]);
		i++;
	}
	free(mtx);
}
