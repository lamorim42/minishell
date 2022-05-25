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
#include "../Libft/ft_strdup.c"
#include "../Libft/ft_memcmp.c"
#include "../Libft/ft_strlcpy.c"
#include "../Libft/ft_strlen.c"
#include "../Libft/ft_arrcmp.c"
#include "../Libft/ft_strncmp.c"
#include "../Libft/ft_free_arr.c"
#include "../includes/minishell.h"

// Tokenizer

TEST(tokenizer, test_tokenizer_ls)
{
	t_line	line;
	line.str = "ls";
	char *esperado[] = {"ls", NULL};
	char	**resultado = tokenizer(&line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
	ft_free_arr(resultado);
}

TEST(tokenizer, test_tokenizer_ls_With_more_cmds)
{
	t_line	line;
	line.str =  "ls -a";
	char	*esperado[] = {"ls", "-a", NULL};
	char	**resultado = tokenizer(&line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
	ft_free_arr(resultado);
}

TEST(tokenizer, test_tokenizer_ls_a_pipe)
{
	t_line	line;
	line.str =  "ls -a|";
	char	*esperado[] = {"ls", "-a", "|", NULL};
	char	**resultado = tokenizer(&line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
	ft_free_arr(resultado);
}

TEST(tokenizer, test_tokenizer_ls_a_pipe_cat_e)
{
	t_line	line;
	line.str =  "ls -a|cat -e";
	char	*esperado[] = {"ls", "-a", "|", "cat", "-e", NULL};
	char	**resultado = tokenizer(&line);

	std::cout << resultado[3] << std::endl;
	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
	ft_free_arr(resultado);
}

TEST(tokenizer, test_tokenizer_ls_pipe_cat)
{
	t_line	line;
	line.str = "ls | cat";
	char	*esperado[] = {"ls", "|", "cat", NULL};
	char	**resultado = tokenizer(&line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
	ft_free_arr(resultado);
}

TEST(tokenizer, test_tokenizer_ls__file_pipe_cat)
{
	t_line	line;
	line.str = "ls > file | cat";
	char	*esperado[] = {"ls", ">", "file", "|", "cat", NULL};
	char	**resultado = tokenizer(&line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
	ft_free_arr(resultado);
}

TEST(tokenizer, test_tokenizer_ls_REDO_file)
{
	t_line	line;
	line.str = "ls >file";
	char	*esperado[] = {"ls", ">", "file", NULL};
	char	**resultado = tokenizer(&line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
	ft_free_arr(resultado);
}

TEST(tokenizer, test_tokenizer_ls_REDA_file)
{
	t_line	line;
	line.str = "ls >> file";
	char	*esperado[] = {"ls", ">>", "file", NULL};
	char	**resultado = tokenizer(&line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
	ft_free_arr(resultado);
}

TEST(tokenizer, test_tokenizer_ls_REDAfile)
{
	t_line	line;
	line.str = "ls >>file";
	char	*esperado[] = {"ls", ">>", "file", NULL};
	char	**resultado = tokenizer(&line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
	ft_free_arr(resultado);
}