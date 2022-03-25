/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 11:19:31 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/25 18:52:00 by dmonteir         ###   ########.fr       */
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

# define SPC 32
# define QUOT 34
# define DQUOT 39

typedef struct s_line {
	char	*pipeline;
	char	**tks;
	char	**lex;
	char	**cpy;
	char	***array_cmds;
	int		nb_pipes;
	int		nb_cmds;
}			t_line;

//tokenizer
char	**token(char *line);
size_t	count_tokens(char *s);
char	*worddup(char **s);
char	*quotdup(char **s,char quot);
int		has_double_quotation(char *str, char quot);

//Lex
char	**lexical_analysis(char **tokens);

// executor
//void	create_cmd(t_line *line);

//grammar
void	syntax_analisys(char	**lex_tokens);

//array
void	ft_free_array(char ***mtx);
int		ft_array_len(char **array);
char	**copy_array(char **tokens, int size);
void	create_cmd(t_line *line);
void	array_cmd(t_line *line);

//Commands
void	count_pipe(t_line *line);

//error
void	return_error(void);
void	syntax_error(void);

//utils
void	print_array(char *msg, char **array);

#endif