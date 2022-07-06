#include <gtest/gtest.h>
#include "../src/tokens/sintax_analysis.c"
#include <stdio.h>

TEST(sintax_analysis, pipe_after_ls_ok)
{
	char **lex = (char **)malloc(sizeof(char **) * 3);
	lex[0] = "WORD";
	lex[1] = "PIPE";
	lex[2] = NULL;

	EXPECT_TRUE(sintax_analysis(lex) == 1);
}

TEST(sintax_analysis, pipe_before_ls_ok)
{
	char **lex = (char **)malloc(sizeof(char **) * 3);
	lex[0] = "PIPE";
	lex[1] = "WORD";
	lex[2] = NULL;

	EXPECT_TRUE(sintax_analysis(lex) == 0);
}

TEST(sintax_analysis, redo_pipe)
{
	char **lex = (char **)malloc(sizeof(char **) * 3);
	lex[0] = "REDO";
	lex[1] = "PIPE";
	lex[2] = NULL;

	EXPECT_TRUE(sintax_analysis(lex) == 0);
}

TEST(sintax_analysis, reda_pipe)
{
	char **lex = (char **)malloc(sizeof(char **) * 3);
	lex[0] = "REDA";
	lex[1] = "PIPE";
	lex[2] = NULL;

	EXPECT_TRUE(sintax_analysis(lex) == 0);
}

TEST(sintax_analysis, redi_pipe)
{
	char **lex = (char **)malloc(sizeof(char **) * 3);
	lex[0] = "REDI";
	lex[1] = "PIPE";
	lex[2] = NULL;

	EXPECT_TRUE(sintax_analysis(lex) == 0);
}

TEST(sintax_analysis, here_pipe)
{
	char **lex = (char **)malloc(sizeof(char **) * 3);
	lex[0] = "HERE";
	lex[1] = "PIPE";
	lex[2] = NULL;

	EXPECT_TRUE(sintax_analysis(lex) == 0);
}

TEST(sintax_analysis, here_pipe_pipe)
{
	char **lex = (char **)malloc(sizeof(char **) * 5);
	lex[0] = "WORD";
	lex[1] = "PIPE";
	lex[2] = "PIPE";
	lex[3] = "WORD";
	lex[4] = NULL;

	EXPECT_TRUE(sintax_analysis(lex) == 0);
}
