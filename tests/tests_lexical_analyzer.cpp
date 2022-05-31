#include <gtest/gtest.h>
#include "../src/tokens/lexical_analyzer.c"
#include <stdio.h>

TEST(lexical_analyzes, ls_to_word)
{
	t_line	line;
	line.tks_nbr = 1;
	line.tks = (char**)malloc(sizeof(char *) * 2);
	line.tks[0] = ft_strdup("ls");
	line.tks[1] = NULL;
	char	*esperado[] = {"WORD", NULL};
	char	**resultado = lexical_analyzer(&line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(lexical_analyzes, ls_to_word_pipe)
{
	t_line	line;
	line.tks_nbr = 2;
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

TEST(lexical_analyzes, ls_to_word_pipe_word)
{
	t_line	line;
	line.tks_nbr = 3;
	line.tks = (char**)malloc(sizeof(char *) * 4);
	line.tks[0] = ft_strdup("ls");
	line.tks[1] = ft_strdup("|");
	line.tks[2] = ft_strdup("ls");
	line.tks[3] = NULL;
	char	*tks[] = {"ls", "|", "ls", NULL};
	char	*esperado[] = {"WORD", "PIPE", "WORD", NULL};
	char	**resultado = lexical_analyzer(&line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(lexical_analyzes, ls_to_word_red_pipe_word)
{
	t_line	line;
	line.tks_nbr = 3;
	line.tks = (char**)malloc(sizeof(char *) * 6);
	line.tks[0] = ft_strdup("ls");
	line.tks[1] = ft_strdup(">");
	line.tks[2] = ft_strdup("file");
	line.tks[3] = ft_strdup("|");
	line.tks[4] = ft_strdup("ls");
	line.tks[5] = NULL;
	char	*tks[] = {"ls", ">", "file", "|", "ls", NULL};
	char	*esperado[] = {"WORD", "REDO", "WORD", "PIPE", "WORD", NULL};
	char	**resultado = lexical_analyzer(&line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(lexical_analyzes, ls_to_word_REDA_word)
{
	t_line	line;
	line.tks_nbr = 3;
	line.tks = (char**)malloc(sizeof(char *) * 4);
	line.tks[0] = ft_strdup("ls");
	line.tks[1] = ft_strdup(">>");
	line.tks[2] = ft_strdup("file");
	line.tks[3] = NULL;
	char	*tks[] = {"ls", ">>" "file", NULL};
	char	*esperado[] = {"WORD", "REDA", "WORD", NULL};
	char	**resultado = lexical_analyzer(&line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(lexical_analyzes, ls_to_word_REDI_word)
{
	t_line	line;
	line.tks_nbr = 3;
	line.tks = (char**)malloc(sizeof(char *) * 4);
	line.tks[0] = ft_strdup("cat");
	line.tks[1] = ft_strdup("<");
	line.tks[2] = ft_strdup("file");
	line.tks[3] = NULL;
	char	*tks[] = {"cat", "<" "file", NULL};
	char	*esperado[] = {"WORD", "REDI", "WORD", NULL};
	char	**resultado = lexical_analyzer(&line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}
