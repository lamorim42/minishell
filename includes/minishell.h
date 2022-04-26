/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 11:19:31 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/26 19:10:00 by dmonteir         ###   ########.fr       */
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

typedef struct s_line {
	char	*str;
	char	**tks;
	char	**lex;
	char	**cmds;
	char	*bin;
	int		tks_nbr;
	char	**envp;
	int		pid;
}				t_line;

int		count_tks(char *line);
void	signals(t_line *line);
char	**tokenizer(t_line *line);
char	**lexical_analyzer(t_line *line);
int		sintax_analysis(char **lex);
char	**clean_tokens(t_line *line);
void	init_line(t_line *line);
void	free_line(t_line *line);
void	exec_path(t_line *line);
void	init_fork(t_line *line);

char	*path_finder(t_line *line);

#endif