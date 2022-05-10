/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 11:19:31 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/10 20:18:18 by dmonteir         ###   ########.fr       */
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

typedef struct s_pipe_list {
	char				*bin;
	char				**args;
	char				**envp;
	struct s_pipe_list	*next;
}	t_pipe_list;

typedef struct s_line {
	char		*str;
	char		**ctks;
	char		**tks;
	int			fd[2];
	char		**lex;
	char		***cmds;
	char		*bin;
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
char	**clean_tokens(t_line *line);
void	init_line(t_line *line);
void	free_line(t_line *line);
void	exec_path(t_line *line, int *i);
void	init_fork(t_line *line, int *i);
char	***creat_cmd(t_line *line, char **ctks);
//linked list
void population_linked_list(t_line *line);
void	add_back_list(t_pipe_list **list, t_pipe_list **node);

char	*path_finder(t_line *line);
void	print_array(char *msg, char **array);
t_pipe_list	*new_node(char *bin, char **args, char** envp);

void	creat_cmd_list(t_line *line);

#endif