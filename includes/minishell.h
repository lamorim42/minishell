/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 11:19:31 by dmonteir          #+#    #+#             */
/*   Updated: 2022/07/23 17:08:38 by lamorim          ###   ########.fr       */
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
# include <sys/stat.h>
# include <fcntl.h>
# include <stdlib.h>

# define SIZE_TABLE 149

// Boolean
# define TRUE 1
# define FALSE 0

// I/O options
# define IN 0
# define OUT 1

typedef struct s_hash_item {
	char				*key;
	char				*value;
	struct s_hash_item	*next;
}	t_hash_item;

typedef struct s_hash_table {
	struct s_hash_item	**item;
	int					size;
	int					count;
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
	int			std_fd[2];
	char		**ctks;
	char		**tks;
	int			fd[2];
	char		**lex;
	int			tks_nbr;
	char		**envp;
	int			status_code;
	int			count_cmds;
	int			*pid;
	int			pid_index;
	int			sig;
	t_pipe_list	*list_cmds;
}				t_line;

struct s_minishell
{
	t_line			*line;
	t_hash_table	*table;
};

//main_loop
void			facade(t_line *line, t_hash_table **table);
void			building_tokens(t_line *line);

void			verification_input(t_pipe_list *temp);
void			close_fds(t_pipe_list *node);
void			open_fds(t_line *line);
int				is_command(t_pipe_list *node);
void			close_std_fd(t_line *line);
int				verify_quots(char *str);
int				is_args_ok(char *arg);
char			*return_var_key(char *str);
void			key_len(char *str, int *i, int *len);
void			insert_var_in_hash(char *arg, t_hash_table **table);
void			insert_simple_var_in_hash(t_hash_table **table,
					char **split_str);
void			building_var(char **split, char *str, char *s);

char			*take_prefix_var(char *str, unsigned int *i);
char			*sufix_var(char *str, unsigned int *i);

int				is_redirect_output(t_pipe_list *node);
int				is_redirect_input(t_pipe_list *node);

char			ft_strcmp_len(char *s1, char *s2);
char			*char_cat(char *str, char c);
void			signal_here(t_line *line);

void			expand_var(t_line *line, t_hash_table *table);

//array
int				ft_array_len(char **array);
char			**copy_array(char **tokens, int size);

int				count_tks(char *line);
void			update_var_env(t_hash_table **table, char *str_key, char *val);

//signals
void			signals(t_line *line);
void			signals_parent(t_line *line);
void			signal_ignore(t_line *line);
void			signal_here(t_line *line);
void			sig_here(int sig);
void			sig_handler(int sig);

int				file_exists(int fd);
char			**tokenizer(t_line *line);
char			**lexical_analyzer(t_line *line);
int				sintax_analysis(char **lex);
void			init_line(t_line *line);

//free
void			free_list(t_pipe_list *list);
void			free_array(char **mtx);
void			free_line(t_line *line);
char			**clean_tokens(t_line *line);
void			free_sintax(t_line *line);
int				count_quots(char *tks);
void			error_msg(char *str, char *msg);
void			free_exit(t_line *line, t_hash_table *table, int status);

//exec
void			init_fork(t_line *line, t_pipe_list *list,
					t_hash_table **table);
char			*path_finder(char *cmd, t_hash_table **table);
void			exec_path(t_line *line, t_pipe_list *list,
					t_hash_table **table);
void			exec_list(t_line *line, t_hash_table **table);
void			creat_list_cmd(t_line *line);
void			list_generation_bin(t_line *line, t_hash_table **table);
int				find_input(t_pipe_list *node);
void			find_output(t_pipe_list *node);

//here_doc
char			*here_doc_buffer(t_pipe_list *node);
void			here_doc_verification(t_line *line);
void			here_doc_write(char *buffer, t_pipe_list *list);
void			creat_here_doc(t_pipe_list *list);
void			close_here_doc(t_pipe_list *list);

//Builtins
int				is_a_builtin(char **node);
void			exec_builtins(t_pipe_list *node, t_hash_table **table);
void			echo_builtin(t_pipe_list *node);
void			cd_builtin(t_pipe_list *node, t_hash_table **table);
void			pwd_builtin(t_hash_table **table);
void			env_builtin(t_pipe_list *node, t_hash_table **table);
void			export_builtin(t_pipe_list *node, t_hash_table **table);
void			unset_builtin(t_pipe_list *node, t_hash_table **table);
void			exit_builtin(t_pipe_list *node, t_hash_table **table);

//linked list
void			add_back_list(t_pipe_list **list, t_pipe_list *node);
void			print_list(t_pipe_list *stack);
t_pipe_list		*new_node(char **args);
void			free_list(t_pipe_list *list);

//hashTable
t_hash_item		*create_item(char *key, char *value);
t_hash_table	*create_table(int size);
void			free_item(t_hash_item *item);
void			free_table(t_hash_table **table);
void			init_hash(void);
void			hash_insert(t_hash_table **table, char *key, char *value);
char			*search_item(t_hash_table *table, char *key);
unsigned long	hash_function(char *str);
void			table_delete(t_hash_table **table, char *key);
t_hash_table	*population_hash_table(t_line *line, t_hash_table **table);
char			*print_search(t_hash_table **table, char *key);
void			print_table(t_hash_table **table);

//Debug functions
void			print_array(char *msg, char **array);
void			print_list(t_pipe_list *stack);

#endif