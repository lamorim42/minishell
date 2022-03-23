/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   procecc_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 16:39:22 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/23 17:37:57 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

char	**copy_array(char **tokens)
{
	char	**array_cpy;
	int		len;
	int		i;


	len = ft_array_len(tokens);
	array_cpy = malloc(sizeof(char *) * (len + 1));

	i = 0;
	while (tokens[i])
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

/* void	create_cmd(t_line *line)
{
	int	i;
	char	**array_cmd;


	i = 0;
	while(line->tks[i])
	{
		if (ft_strchr(line->tks[i], "|"))
		{
			*array_cmd =
		}
	}

} */
