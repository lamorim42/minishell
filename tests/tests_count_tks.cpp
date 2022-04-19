/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_count_tks.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 20:17:11 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/19 19:08:34 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//# include "minishell.h"
#include <gtest/gtest.h>
#include "../Libft/ft_strchr.c"
#include "../src/tokens/count_tks.c"
#include <iostream>

TEST(count_tks, test_count_words)
{
	std::string	line;
	int esperado;
	int resultado;

	line = "miau um dois";
	esperado = 3;
	resultado = count_tks((char *)line.c_str());

	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_count_words_with_commands)
{
	std::string	line;
	int esperado;
	int resultado;

	line = "ls -a | tr a b | wc -l";
	esperado = 9;
	resultado = count_tks((char *)line.c_str());

	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_count_words_with_commands_juntinhos)
{
	std::string	line;
	int esperado;
	int resultado;

	line = "ls|cat -e";
	esperado = 4;
	resultado = count_tks((char *)line.c_str());

	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_count_words_with_commands_junt)
{
	std::string	line;
	int esperado;
	int resultado;

	line = "ls|cat-e";
	esperado = 3;
	resultado = count_tks((char *)line.c_str());

	ASSERT_EQ(resultado, esperado);
}


TEST(count_tks, test_count_tks_with_quots)
{
	std::string	line;
	int esperado;
	int resultado;

	line = "\"l\"s | cat -e";
	esperado = 4;
	resultado = count_tks((char *)line.c_str());

	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_count_tks_with_quots_return_1)
{
	std::string	line;
	int esperado;
	int resultado;

	line = "\"ls -a\"";
	esperado = 1;
	resultado = count_tks((char *)line.c_str());

	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_count_tks_with_quots_return_2)
{
	std::string	line;
	int esperado;
	int resultado;

	line = "\"ls\" -a";
	esperado = 2;
	resultado = count_tks((char *)line.c_str());

	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_count_tks_with_quots_end_of_word)
{
	std::string	line;
	int esperado;
	int resultado;

	line = "l\"s\" -a";
	esperado = 2;
	resultado = count_tks((char *)line.c_str());

	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_count_tks_with_quots_incomplite)
{
	std::string	line;
	int esperado;
	int resultado;

	line = "l\"s -a";
	esperado = 2;
	resultado = count_tks((char *)line.c_str());

	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_count_tks_with_quots_incomplite_first)
{
	// Assinement
	std::string	line;
	int esperado;
	int resultado;
	line = "\"ls -a";
	esperado = 1;

	// act
	resultado = count_tks((char *)line.c_str());

	//assert
	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_count_tks_with_quots_echo)
{
	// Assinement
	std::string	line;
	int esperado;
	int resultado;
	line = "echo \"hello world shell\"";
	esperado = 2;

	// act
	resultado = count_tks((char *)line.c_str());

	//assert
	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_count_tks_with_quots_with_out_par_echo)
{
	// Assinement
	std::string	line;
	int esperado;
	int resultado;
	line = "echo \"abba";
	esperado = 2;

	// act
	resultado = count_tks((char *)line.c_str());

	//assert
	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_cat_pipe_cat_with_quots)
{
	// Assinement
	std::string	line;
	int esperado;
	int resultado;
	line = "ca\"t\"|\" cat\"|";
	esperado = 4;

	// act
	resultado = count_tks((char *)line.c_str());

	//assert
	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_count_tks_with_squots_return_2)
{
	std::string	line;
	int esperado;
	int resultado;

	line = "\'ls\' -a";
	esperado = 2;
	resultado = count_tks((char *)line.c_str());

	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_count_tks_with_squots_return_1)
{
	std::string	line;
	int esperado;
	int resultado;

	line = "\'ls -a\'";
	esperado = 1;
	resultado = count_tks((char *)line.c_str());

	ASSERT_EQ(resultado, esperado);
}

TEST(count_tks, test_count_tks_with_squots_and_quots)
{
	std::string	line;
	int esperado;
	int resultado;

	line = "echo \"abba \'ebba\' obba\"";
	esperado = 2;
	resultado = count_tks((char *)line.c_str());
	//dprintf(2, "resultado = %d\n", resultado);

	ASSERT_EQ(resultado, esperado);
}

int	main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return(RUN_ALL_TESTS());
}
