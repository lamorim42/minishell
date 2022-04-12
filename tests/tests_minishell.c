/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_minishell.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:17:11 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/12 17:31:55 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//# include "minishell.h"
# include "criterion/criterion.h"
# include <stdio.h>
int count_tks(char *line);

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
	esperado = 4;
	resultado = count_tks(line);
	printf("%d\n", resultado);

	cr_assert_eq(resultado, esperado);
}