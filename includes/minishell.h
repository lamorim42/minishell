/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 11:19:31 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/28 19:15:44 by dmonteir         ###   ########.fr       */
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
# include <fcntl.h>
#include <stdlib.h>

//#define size_table 19211
#define size_table 149

typedef struct s_hash_item {
	char* key;
	char* value;
	struct s_hash_item *next;
}	t_hash_item;

typedef struct s_hash_table {
	struct s_hash_item **item;
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
	int			sig;
	t_pipe_list	*list_cmds;
}				t_line;

int			ft_array_len(char **array);
int			count_tks(char *line);
void		signals(t_line *line);
char		**tokenizer(t_line *line);
char		**lexical_analyzer(t_line *line);
int			sintax_analysis(char **lex);
void		init_line(t_line *line);

//free
void		free_line(t_line *line);
char		**clean_tokens(t_line *line);
void		free_list(t_pipe_list *list);
void		init_hash();
void		free_array(char **mtx);
void		free_line(t_line *line);
char		**clean_tokens(t_line *line);

//exec
void		init_fork(t_line *line, t_pipe_list *list);
char		*path_finder(t_line *line, char *cmd);
void		exec_path(t_line *line, t_pipe_list *list);
void		population_linked_list(t_line *line);
void		exec_list(t_line *line);
void		creat_cmd_list(t_line *line);
void		list_generation_bin(t_line *line);
char		***creat_cmd(t_line *line, char **ctks);

//Builtins
int		is_a_builtin(char **node);
void	exec_builtins(t_pipe_list *node);
void	echo_builtin(t_pipe_list *node);

//linked list
void		add_back_list(t_pipe_list **list, t_pipe_list **node);
void		print_list(t_pipe_list *stack);
t_pipe_list	*new_node(char **args);
void		free_list(t_pipe_list *list);

//hashTable
t_hash_item	*create_item(char *key, char *value);
t_hash_table	*create_table(int size);
void	free_item(t_hash_item *item);
void	free_table(t_hash_table **table);
void	init_hash();
void	hash_insert(t_hash_table **table, char *key, char *value);
char	*search_item(t_hash_table **table, char *key);
unsigned long	hash_function(char *str);
void	table_delete(t_hash_table **table, char *key);

char	*path_finder(t_line *line, char *cmd);

//Debug functions
void	print_array(char *msg, char **array);
void	print_list(t_pipe_list *stack);


#endif