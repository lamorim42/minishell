/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 11:19:31 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/18 10:16:30 by dmonteir         ###   ########.fr       */
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

# define SEP "|"

typedef struct s_node
{
 	char	*str;
	struct s_node *next;
}	t_node;

typedef struct s_stack
{
	t_node *array;
}	t_stack;

void	parse(char *line);
char	*split_line(char *line);
char	*worddup(const char *s, size_t len);
int		has_double_quotation(char *str, char quot);
char	*handle_quotes(char **ptr_len, char quot);
void	return_error(void);
char	*handle_space(char **ptr_len);

//linked list
t_node	*new_node(char *str);
void	add_back_stack(char *content, t_node **head);
void	print_list(t_stack **stack);
char	*handle_first_arg(char **ptr_len);

#endif