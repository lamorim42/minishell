/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:12:17 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/12 20:47:00 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	facade(t_line *line, t_hash_table **table);
static void	control_d(char *str, t_hash_table **table);
static void	building_tokens(t_line *line);
static void	exec_pipe_line(t_line *line, t_hash_table **table);
static void	alloc_commands(t_line *line);

int	main(int argc, char **argv, char **envp)
{
	t_line			line;
	t_hash_table	*table;

	table = NULL;

	if (argv[0] == NULL && argc > 1)
		return (1);
	init_line(&line);
	line.envp = envp;
	population_hash_table(&line, &table);
	facade(&line, &table);
	return (0);
}

static void	facade(t_line *line, t_hash_table **table)
{
	while (1)
	{
		signals(line);
		line->str = readline("miau> ");
		control_d(line->str, table);
		if (line->str != NULL && ft_strlen(line->str) > 0)
		{
			building_tokens(line);
			if (!sintax_analysis(line->lex) || (count_quots(line->str) % 2 != 0))
				free_sintax(line);
			else
				exec_pipe_line(line, table);
		}
		else
			free(line->str);
	}
}

static void	control_d(char *str, t_hash_table **table)
{
	if (str == NULL)
	{
		free_table(table);
		write(2, "exit\n", 6);
		exit(0);
	}
}

static void	building_tokens(t_line *line)
{
	line->tks_nbr = count_tks(line->str);
	line->tks = tokenizer(line);
	line->lex = lexical_analyzer(line);
}

static void	exec_pipe_line(t_line *line, t_hash_table **table)
{
	line->ctks = clean_tokens(line);
	expand_var(line, (*table));
	creat_cmd(line);
	signal_here(line);
	here_doc_verification(line);
	list_generation_bin(line);
	alloc_commands(line);
	open_fds(line);
	exec_list(line, table);
	add_history(line->str);
	free_line(line);
}

static void	alloc_commands(t_line *line)
{
	t_pipe_list *temp;

	temp = line->list_cmds;
	while (temp)
	{
		if (is_command(temp))
			line->count_cmds++;
		temp = temp->next;
	}

	line->pid = ft_calloc((line->count_cmds + 1), sizeof(int));
	if (!line->pid)
		return ;
}

void	open_fds(t_line *line)
{
	t_pipe_list	*list;

	list = line->list_cmds;
	while (list)
	{
		if (!ft_strncmp(list->args[0], "REDI", 4))
		{
			list->fd[0] = open(list->args[1], O_RDONLY);
			if (list->fd[0] == -1)
			{
				perror(list->args[1]);
				free_line(line);
			}
		}
		else if (!ft_strncmp(list->args[0], "REDO", 4))
		{
			list->fd[0] = open(list->args[1], O_RDWR | O_CREAT | O_TRUNC, 0644);
			if (list->fd[0] == -1)
			{
				perror(list->args[1]);
				free_line(line);
			}
		}
		else if (!ft_strncmp(list->args[0], "REDA", 4))
		{
			list->fd[0] = open(list->args[1], O_RDWR | O_CREAT
							| O_APPEND, 0644);
			if (list->fd[0] == -1)
			{
				perror(list->args[1]);
				free_line(line);
			}
		}
		else if (!ft_strncmp(list->args[0], "PIPE", 4))
		{
			if (pipe(list->fd) != 0)
				dprintf(2, "pipe error\n");
		}
		list = list->next;
	}

}

void	here_doc_verification(t_line *line)
{
	t_pipe_list *temp;
	char	*buffer;

	temp = line->list_cmds;
	buffer = NULL;
	while(temp)
	{
		if (!ft_strncmp(temp->args[0], "HERE", 4))
		{
			buffer = here_doc_buffer(temp);
			here_doc_write(buffer, temp);
			free(buffer);
		}
		temp = temp->next;
	}
}

void	here_doc_write(char *buffer, t_pipe_list *list)
{
	if (pipe(list->fd) != 0)
		dprintf(2, "pipe error\n");
	if (buffer)
		write(list->fd[1], buffer, ft_strlen(buffer));
	close(list->fd[1]);
}

t_hash_table	*population_hash_table(t_line *line, t_hash_table **table)
{
	int	i;
	char	**var;
	i = 0;
	(*table) = create_table(SIZE_TABLE);
	while(line->envp[i])
	{
		var = ft_split(line->envp[i], '=');
		if (var[0] != NULL && var[1] != NULL)
			hash_insert(table, var[0], var[1]);
		else if (var[0] != NULL)
			hash_insert(table, var[0], "");
		ft_free_arr(var);
		i++;
	}
	hash_insert(table, "?", "0");
	return (*table);
}

char	ft_strcmp_len(char *s1, char *s2)
{
	if (ft_strlen(s1) == ft_strlen(s2))
		return (ft_strncmp(s1, s2, __UINT64_MAX__) == 0);
	return (0);
}
