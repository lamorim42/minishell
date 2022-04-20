/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_tokenizer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                 +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/16 21:07:05 by lamorim           #+#    #+#             */
/*   Updated: 2022/04/19 20:36:40 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include <iostream>
#include "../src/tokens/tokenizer.c"
#include "../Libft/ft_memcmp.c"
#include "../Libft/ft_strlcpy.c"
#include "../Libft/ft_strlen.c"
#include "../Libft/ft_arrcmp.c"
#include "../Libft/ft_strncmp.c"
#include "../Libft/ft_free_arr.c"

// Tokenizer

TEST(tokenizer, test_tokenizer_ls)
{
	char *line = "ls";
	char *esperado[] = {"ls", NULL};
	char	**resultado = tokenizer(line);

	std::cout << "testando" << std::endl;
	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
	ft_free_arr(resultado);
}

TEST(tokenizer, test_tokenizer_ls_With_more_cmds)
{
	char	*line = "ls -a";
	char	*esperado[] = {"ls", "-a", NULL};
	char	**resultado = tokenizer(line);


	std::cout << "testando2" << std::endl;
	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
	ft_free_arr(resultado);
}


TEST(tokenizer, test_tokenizer_ls_a_pipe)
{
	char	*line = "ls -a|";
	char	*esperado[] = {"ls", "-a", "|", NULL};
	char	**resultado = tokenizer(line);

	std::cout << "testando3" << std::endl;
	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
	ft_free_arr(resultado);
}