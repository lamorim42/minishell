/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 11:19:31 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/22 16:17:27 by lamorim          ###   ########.fr       */
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

//tokenizer
char	**token(char *line);
size_t	count_words(char *s);
char	*worddup(char **s);
char	*quotdup(char **s);
int	has_double_quotation(char *str, char quot);

//Lex
char	**tokens_classification(char **tokens);

//error
void	return_error(void);
#endif