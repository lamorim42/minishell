/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 11:19:31 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/06 09:11:57 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <error.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <string.h>
# include "../Libft/libft.h"
# include <sys/wait.h>
# include <sys/types.h>
# define SPC 32
# define QUOT 34
# define DQUOT 39

typedef struct s_line {
	char	*pipeline;
	char	**tks;
	char	**lex;
	char	**cpy;
	char	***array_cmds;
	char	**cmds_table;
	int		nb_pipes;
	int		nb_cmds;
	char	**argv;
	char	**envp;
	int		argc;
	char	**path;
	char	**bin;
	char	**map_cmds;
	int		fd[2];
	int		*array_pid;
}			t_line;

//tokenizer
char	**get_tokens(char *line);
char	*worddup(char **s);
int		has_double_quotation(char *str, char quot);
char	*quotdup(char **s, char	quot);

//Lex
char	**lexical_analysis(char **tokens);

// executor
//void	create_cmd(t_line *line);
void	ft_free_arrcmds(char ***mtx);
char	**check_path(t_line *line);
void	exec_cmd(t_line *line, int i, int j);
void	ft_fork(t_line *line, int i, int j);
void	ft_dup_pipe(t_line *line, int i);
void	exec_cmd_pipe(t_line *line);
void	get_pid(t_line *line, int pid, int i);
void	get_fd(t_line *line, int i);
void	dup_in(t_line *line);
void	dup_out(t_line *line);

//path
char	*get_path(t_line *line);
void	split_path(t_line *line);
void	str_cmd(t_line *line);

//grammar
void	syntax_analisys(char	**lex_tokens);

//array
void	ft_free_array(char **mtx);
int		ft_array_len(char **array);
char	**copy_array(char **tokens, int size);
void	array_cmd(t_line *line);

//Commands
void	create_cmd_arr(t_line *line);
void	create_cmd_table(t_line *line);

//error
void	return_error(void);

//utils
void	print_array(char *msg, char **array);
void	init_array_pid(t_line *line);
void	init_array_fd(t_line *line);
#endif