/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creat_cmd_table.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/02 20:10:01 by lamorim           #+#    #+#             */
/*   Updated: 2022/07/03 18:54:42 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	expand_var(t_line *line, t_hash_table *table);
static char	*return_var_key(char *str);
static char	*join_str_value(char *key, char *value, char *str);

static void	counter_pipes(int *count_pipes, char **temp_lex)
{
	int	i;

	i = 0;
	while (temp_lex && temp_lex[i])
	{
		if (!ft_strncmp(temp_lex[i], "PIPE", 4))
			(*count_pipes)++;
		i++;
	}
}

static char	***creat_cmd_table(t_line *line)
{
	struct s_aux	aux;

	init_aux(&aux, line);
	counter_pipes(&aux.count_pipes, aux.lex);
	line->cmds = ft_calloc((aux.count_pipes * 2) + 3, (sizeof(char **)));
	if (line->cmds == NULL)
		return (NULL);
	while (aux.ctks && aux.lex[aux.i])
	{
		if ((!ft_strncmp(aux.lex[aux.i], "WORD", 4))
			|| (!ft_strncmp(aux.lex[aux.i], "VAR", 3)))
			aux.count_words++;
		update_cmd_table(line, &aux);
		aux.i++;
	}
	line->cmds[aux.j] = NULL;
	return (line->cmds);
}

void	creat_cmd(t_line *line, t_hash_table **table)
{
	line->ctks = clean_tokens(line);
	expand_var(line, (*table));
	line->cmds = creat_cmd_table(line);
}

static void	expand_var(t_line *line, t_hash_table *table)
{
	char	*value;
	char	*key;
	int		i;

	i = 0;
	while(line->ctks[i])
	{
		while (ft_strchr(line->ctks[i], '$') != NULL)
		{
			key = return_var_key(line->ctks[i]);
			value = search_item(table, key);
			printf("value depois da search = %s\n", value);
			if (value != NULL)
			{
				line->ctks[i] = join_str_value(key, value, line->ctks[i]);
				free (value);
			}
			else
				line->ctks[i] = ft_strdup("");
			free (key);
		}
		i++;
	}
}

static char *join_str_value(char *key, char *value, char *str)
{
	unsigned int	i;
	unsigned int	j;
	char	*sufix_str;
	char	*new_str;
	char	*temp;

	i = 0;
	sufix_str = ft_strdup("");
	while (str[i] && str[i] != '$')
	{
		temp = sufix_str;
		sufix_str = char_cat(sufix_str, str[i]);
		free(temp);
		i++;
	}
	if ((ft_strlen(key) + 1) == (ft_strlen(str)))
	{
		free(sufix_str);
		free(str);
		return (ft_strdup(value));
	}
	else if (i == 0)
	{
		temp = ft_strdup(&str[(ft_strlen(key) + 1)]);
		new_str = ft_strjoin(value, temp);
		free(sufix_str);
		free(temp);
		free(str);
		return (new_str);
	}
	else if(i > 0)
	{
		j = 0;
		if (str[i + (ft_strlen(key) + 1)] != '\0')
		{
			new_str = ft_strjoin(sufix_str, value);
			while(str[i + (ft_strlen(key) + 1 + j)])
				j++;
			free(sufix_str);
			sufix_str = new_str;
			temp = ft_substr(str, i + (unsigned int)ft_strlen(key) + 1, (size_t)j);
			new_str = ft_strjoin(sufix_str, temp);
			free(sufix_str);
			free(temp);
			free(str);
			return(new_str);
		}
		else
		{
			new_str = ft_strjoin(sufix_str, value);
			free(sufix_str);
			free(str);
			return(new_str);
		}
	}
	return (NULL);
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

static char *return_var_key(char *str)
{
	int		i;
	int		len;
	int		cpy;
	char	*key;

	i = 0;
	len = 0;
	cpy = 0;
	key = NULL;
	while (str[i])
	{
		if (str[i] == '$')
		{
			cpy = i + 1;
			while (str[i] && str[i] != ' ')
			{
				len++;
				i++;
			}
			break ;
		}
		else
			i++;
	}

	key = ft_substr(str, (unsigned int)cpy, ((size_t)len - 1));
	return (key);
}

//Quando expandimos duas variaveis dentro ou fro de aspas da erro
//echo "$USER ahudaus $USER"