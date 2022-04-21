#include <gtest/gtest.h>
#include "../src/tokens/lexical_analyzer.c"
//#include "../Libft/ft_arrcmp.c"
#include <stdio.h>

TEST(lexical_analyzes, ls_to_word)
{
	t_line	line;
	line.tks = (char**)malloc(sizeof(char *) * 2);
	line.tks[0] = ft_strdup("ls");
	line.tks[1] = NULL;
	char	*esperado[] = {"WORD", NULL};
	char	**resultado = lexical_analyzer(&line);
	int		i = 0;

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(lexical_analyzes, ls_to_word_pipe)
{
	t_line	line;
	line.tks = (char**)malloc(sizeof(char *) * 3);
	line.tks[0] = ft_strdup("ls");
	line.tks[1] = ft_strdup("|");
	line.tks[2] = NULL;
	char	*tks[] = {"ls |", NULL};
	char	*esperado[] = {"WORD", "PIPE", NULL};
	char	**resultado = lexical_analyzer(&line);
	int		i = 0;

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}