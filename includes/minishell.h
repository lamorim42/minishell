/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 11:19:31 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/23 12:23:27 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../Libft/libft.h"

typedef struct s_line {
	char	*str;
	char	**tks;
	char	**lex;
	char	**cmds;
	char	*bin;
	int		tks_nbr;
	char	**envp;
}			t_line;

int		count_tks(char *line);
char	**tokenizer(t_line *line);
char	**lexical_analyzer(t_line *line);
int		sintax_analysis(char **lex);
char	**clean_tokens(t_line *line);

char	*path_finder(t_line *line);

#endif