/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_count_tks.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:17:11 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/16 21:07:10 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//# include "minishell.h"
#include <criterion/criterion.h>
#include <stdio.h>
#include "minishell.h"

Test(count_tks, test_count_words)
{
	char *line;
	int esperado;
	int resultado;

	line = "miau um dois";
	esperado = 3;
	resultado = count_tks(line);

	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_words_with_commands)
{
	char *line;
	int esperado;
	int resultado;

	line = "ls -a | tr a b | wc -l";
	esperado = 9;
	resultado = count_tks(line);

	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_words_with_commands_juntinhos)
{
	char *line;
	int esperado;
	int resultado;

	line = "ls|cat -e";
	esperado = 4;
	resultado = count_tks(line);

	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_words_with_commands_junt)
{
	char *line;
	int esperado;
	int resultado;

	line = "ls|cat-e";
	esperado = 3;
	resultado = count_tks(line);

	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_tks_with_quots)
{
	char *line;
	int esperado;
	int resultado;

	line = "\"l\"s | cat -e";
	esperado = 4;
	resultado = count_tks(line);

	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_tks_with_quots_return_1)
{
	char *line;
	int esperado;
	int resultado;

	line = "\"ls -a\"";
	esperado = 1;
	resultado = count_tks(line);

	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_tks_with_quots_return_2)
{
	char *line;
	int esperado;
	int resultado;

	line = "\"ls\" -a";
	esperado = 2;
	resultado = count_tks(line);

	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_tks_with_quots_end_of_word)
{
	char *line;
	int esperado;
	int resultado;

	line = "l\"s\" -a";
	esperado = 2;
	resultado = count_tks(line);

	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_tks_with_quots_incomplite)
{
	char *line;
	int esperado;
	int resultado;

	line = "l\"s -a";
	esperado = 2;
	resultado = count_tks(line);

	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_tks_with_quots_incomplite_first)
{
	// Assinement
	char *line;
	int esperado;
	int resultado;
	line = "\"ls -a";
	esperado = 1;

	// act
	resultado = count_tks(line);

	//assert
	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_tks_with_quots_echo)
{
	// Assinement
	char *line;
	int esperado;
	int resultado;
	line = "echo \"hello world shell\"";
	esperado = 2;

	// act
	resultado = count_tks(line);

	//assert
	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_tks_with_quots_with_out_par_echo)
{
	// Assinement
	char *line;
	int esperado;
	int resultado;
	line = "echo \"abba";
	esperado = 2;

	// act
	resultado = count_tks(line);

	//assert
	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_cat_pipe_cat_with_quots)
{
	// Assinement
	char *line;
	int esperado;
	int resultado;
	line = "ca\"t\"|\" cat\"|";
	esperado = 4;

	// act
	resultado = count_tks(line);

	//assert
	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_tks_with_squots_return_2)
{
	char *line;
	int esperado;
	int resultado;

	line = "\'ls\' -a";
	esperado = 2;
	resultado = count_tks(line);

	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_tks_with_squots_return_1)
{
	char *line;
	int esperado;
	int resultado;

	line = "\'ls -a\'";
	esperado = 1;
	resultado = count_tks(line);

	cr_assert_eq(resultado, esperado);
}

Test(count_tks, test_count_tks_with_squots_and_quots)
{
	char *line;
	int esperado;
	int resultado;

	line = "echo \"abba \'ebba\' obba\"";
	esperado = 2;
	resultado = count_tks(line);
	//dprintf(2, "resultado = %d\n", resultado);

	cr_assert_eq(resultado, esperado);
}
