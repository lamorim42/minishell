/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_tokenizer.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/16 21:07:05 by lamorim           #+#    #+#             */
/*   Updated: 2022/04/16 21:07:06 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <criterion/criterion.h>
#include <stdio.h>
#include "minishell.h"

// Tokenizer

Test(tokenizer, test_tokenizer_ls)
{
	char	*line = "ls";
	char	*esperado[] = {"ls", NULL};
	char	**resultado = tokenizer(line);

	cr_assert_str_eq(resultado[0], esperado[0]);
}

Test(tokenizer, test_tokenizer_ls_With_more_cmds)
{
	char	*line = "ls -a";
	char	*esperado[] = {"ls", "-a", NULL};
	char	**resultado = tokenizer(line);

	cr_assert_str_eq(resultado[0], esperado[0]);
//	cr_assert_str_eq(resultado[1], esperado[1]);
}

Test(tokenizer, test_tokenizer_ls_a_pipe)
{
	char	*line = "ls -a|";
	char	*esperado[] = {"ls", "-a", "|", NULL};
	char	**resultado = tokenizer(line);

	cr_assert_str_eq(resultado[0], esperado[0]);
//	cr_assert_str_eq(resultado[1], esperado[1]);
//	cr_assert_str_eq(resultado[2], esperado[2]);
}