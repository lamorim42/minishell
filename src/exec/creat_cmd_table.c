/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creat_cmd_table.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/02 20:10:01 by lamorim           #+#    #+#             */
/*   Updated: 2022/07/21 20:14:19 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*return_var_key(char *str);
static char	*join_str_value(char *key, char *value, char *str);
static char	*take_prefix_var(char *str, unsigned int *i);
static void	change_value(t_line *line, t_hash_table *table, unsigned int i);
static char	*sufix_var(char *str, unsigned int *i);
char	**make_args(t_line *line, int *index);
int		get_arg_size(t_line *line, int index);

void	creat_list_cmd(t_line *line)
{
	int			i;

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
	return(arg);
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

void	expand_var(t_line *line, t_hash_table *table)
{

	int		i;

	i = 0;
	while (line->lex[i])
	{
		if (ft_strcmp_len(line->lex[i], "VAR") && line->tks[i][0] != '\'')
			while(ft_strchr(line->tks[i], '$'))
				change_value(line, table, i);
		i++;
	}
}

static void	change_value(t_line *line, t_hash_table *table, unsigned int i)
{
	char	*value;
	char	*key;
	char	*temp;

	key = return_var_key(line->tks[i]);
	value = search_item(table, key);
	if (value != NULL)
	{
		temp = line->tks[i];
		line->tks[i] = join_str_value(key, value, line->tks[i]);
		free(temp);
		free (value);
	}
	else
	{
		temp = line->tks[i];
		line->tks[i] = ft_strdup("");
		free(temp);
	}
	free (key);
}

static char *join_str_value(char *key, char *value, char *str)
{
	unsigned int	i;
	char			*concat_str;
	char			*new_str;
	char			*temp;

	i = 0;
	concat_str = NULL;
	temp = NULL;
	new_str = NULL;
	if ((ft_strlen(key) + 1) == (ft_strlen(str)))
		return (ft_strdup(value));
	new_str = take_prefix_var(str, &i);
	if (new_str != NULL)
	{
		temp = new_str;
		new_str = ft_strjoin(new_str, value);
		free(temp);
		temp = NULL;
	}
	else
		new_str = ft_strdup(value);
	i += (ft_strlen(key) + 1);
	if(str[i])
		concat_str = sufix_var(str, &i);
	if (concat_str != NULL)
	{
		temp = new_str;
		new_str = ft_strjoin(new_str, concat_str);
		free(temp);
		free(concat_str);
	}
	return (new_str);
}

static char	*take_prefix_var(char *str, unsigned int *i)
{
	char	*sufix;

	sufix = NULL;
	while (str[*i] && str[*i] != '$')
	{
		(*i)++;
	}

	if (*i > 0)
		sufix = ft_substr(str, 0, *i);
	return sufix;
}

static char	*sufix_var(char *str, unsigned int *i)
{
	char	*temp;
	int		len;

	temp = &str[*i];
	len = ft_strlen(temp);


	if (len > 0)
		return ft_substr(str, *i, len);
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

static char	*return_var_key(char *str)
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
			while (str[i] && !ft_strchr("\"\',:./[]{}=-+*!@# ", str[i]))
			{
				len++;
				i++;
			}
			len--;
			break ;
		}
		else
			i++;
	}

	key = ft_substr(str, (unsigned int)cpy, ((size_t)len));
	return (key);
}
