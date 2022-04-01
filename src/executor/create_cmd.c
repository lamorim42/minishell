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


void	create_cmd_arr(t_line *line)
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


void	create_cmd_table(t_line *line)
{
	int	size;
	int	len;
	char	*temp;

	size = 0;
	line->cmds_table = malloc(sizeof(char **) * (line->nb_pipes + 2));
	temp = line->pipeline;
	while (*temp)
	{
		len = 0;
		while (*temp && *temp == ' ')
			temp++;
		while (temp[len] && temp[len] != '|')
			len++;
		line->cmds_table[size] = ft_substr(temp, 0, len);
		size++;
		temp += len + (temp[len] != '\0');
	}
	line->cmds_table[size] = NULL;

	print_array("array_cpy", line->cmds_table);

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
