/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 11:19:31 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/23 10:11:05 by dmonteir         ###   ########.fr       */
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
	char	**bin;
	int		tks_nbr;
}			t_line;

int		count_tks(char *line);
char	**tokenizer(t_line *line);
char	**lexical_analyzer(t_line *line);
int		sintax_analysis(char **lex);
char	**clean_tokens(t_line *line);

#endif