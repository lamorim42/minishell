/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 11:19:31 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/15 20:43:34 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../Libft/libft.h"
# include <sys/types.h>
# include <sys/wait.h>
# include <signal.h>

//#define size_table 19211
#define size_table 149

typedef struct s_hash_item {
	char* key;
	char* value;
}	t_hash_item;

typedef struct s_hash_table {
	t_hash_item** item;
	int size;
	int count;
}	t_hash_table;

typedef struct s_pipe_list {
	char				**args;
	union
	{
		char			*bin;
		int				fd[2];
	};
	struct s_pipe_list	*next;
	struct s_pipe_list	*prev;
}	t_pipe_list;

typedef struct s_line {
	char		*str;
	char		**ctks;
	char		**tks;
	int			fd[2];
	char		**lex;
	char		***cmds;
	int			tks_nbr;
	char		**envp;
	int			pid;
	t_pipe_list	*list_cmds;
}				t_line;

int ft_array_len(char **array);
int		count_tks(char *line);
void	signals(t_line *line);
char	**tokenizer(t_line *line);
char	**lexical_analyzer(t_line *line);
int		sintax_analysis(char **lex);

void	init_line(t_line *line);
//free
void	free_line(t_line *line);
char	**clean_tokens(t_line *line);
void	free_list(t_pipe_list *list);
void	init_hash();

//exec
void	init_fork(t_line *line, t_pipe_list *list);
void	exec_path(t_line *line, t_pipe_list *list);
void	exec_list(t_line *line);

char	***creat_cmd(t_line *line, char **ctks);

//bin
void	list_generation_bin(t_line *line);
//linked list
void	population_linked_list(t_line *line);
void	add_back_list(t_pipe_list **list, t_pipe_list **node);
void	print_list(t_pipe_list *stack);
t_pipe_list	*new_node(char **args);


char	*path_finder(t_line *line, char *cmd);
void	print_array(char *msg, char **array);

void	creat_cmd_list(t_line *line);

#endif