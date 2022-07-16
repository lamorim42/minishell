/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creat_cmd_table.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/02 20:10:01 by lamorim           #+#    #+#             */
/*   Updated: 2022/07/16 18:10:36 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*return_var_key(char *str);
static char	*join_str_value(char *key, char *value, char *str);

char	**make_args(t_line *line, int *index);
int	get_arg_size(t_line *line, int index);

void	creat_cmd(t_line *line)
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
	if (ft_strncmp(line->lex[*index], "WORD", 4) && ft_strncmp(line->lex[*index], "VAR", 3))
	{
		free(arg[0]);
		arg[0] = ft_strdup(line->lex[*index]);
	}
	*index += size;
	return(arg);
}

int	get_arg_size(t_line *line, int index)
{
	int		size;

	size = 0;
	if (!ft_strncmp(line->lex[index], "WORD", 4) || !ft_strncmp(line->lex[index], "VAR", 3))
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

void	expand_var(t_line *line, t_hash_table *table)
{
	char	*value;
	char	*key;
	int		i;
	char	*temp;

	temp = NULL;
	i = 0;
	while(line->ctks[i])
	{
		while (ft_strchr(line->ctks[i], '$') != NULL
		&& ft_strncmp(line->tks[i], "\'", 1) && !ft_strncmp(line->ctks[i], "$", 1))
		{
			key = return_var_key(line->ctks[i]);
			value = search_item(table, key);
			if (value != NULL)
			{
				temp = line->ctks[i];
				line->ctks[i] = join_str_value(key, value, line->ctks[i]);
				free(temp);
				free (value);
			}
			else
			{
				temp = line->ctks[i];
				line->ctks[i] = ft_strdup("");
				free(temp);
			}
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