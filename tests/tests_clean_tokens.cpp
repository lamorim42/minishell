#include <gtest/gtest.h>
#include "../src/tokens/clean_tokens.c"
#include "../Libft/ft_strtrim.c"
#include "../Libft/ft_bzero.c"
#include "../src/init.c"
#include <stdio.h>


TEST(clean_tokens, delete_quot)
{
	t_line *line;

	line = (t_line*)malloc(sizeof(t_line));
	init_line(line);
	line->tks = (char **)malloc(sizeof(char **) * 2);
	line->tks[0] = "\"ls\"";
	line->tks[1] = NULL;

	char *esperado[] = {"ls", NULL};
	char **resultado = clean_tokens(line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(clean_tokens, delete_quot_in_midle)
{
	t_line *line;

	line = (t_line*)malloc(sizeof(t_line));
	init_line(line);
	line->tks = (char **)malloc(sizeof(char **) * 2);
	line->tks[0] = "\"l\"s";
	line->tks[1] = NULL;

	char *esperado[] = {"ls", NULL};
	char **resultado = clean_tokens(line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(clean_tokens, delete_quot_in_final)
{
	t_line *line;

	line = (t_line*)malloc(sizeof(t_line));
	init_line(line);
	line->tks = (char **)malloc(sizeof(char **) * 2);
	line->tks[0] = "l\"s\"";
	line->tks[1] = NULL;

	char *esperado[] = {"ls", NULL};
	char **resultado = clean_tokens(line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(clean_tokens, delete_quot_in_final_2)
{
	t_line *line;

	line = (t_line*)malloc(sizeof(t_line));
	init_line(line);
	line->tks = (char **)malloc(sizeof(char **) * 2);
	line->tks[0] = "ls\"\"";
	line->tks[1] = NULL;



	char *esperado[] = {"ls", NULL};
	char **resultado = clean_tokens(line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(clean_tokens, no_quots)
{
	t_line *line;

	line = (t_line*)malloc(sizeof(t_line));
	init_line(line);
	line->tks = (char **)malloc(sizeof(char **) * 2);
	line->tks[0] = "ls";
	line->tks[1] = NULL;

	char *esperado[] = {"ls", NULL};
	char **resultado = clean_tokens(line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}


TEST(clean_tokens, simple_quots)
{
	t_line *line;

	line = (t_line*)malloc(sizeof(t_line));
	init_line(line);
	line->tks = (char **)malloc(sizeof(char **) * 2);
	line->tks[0] = "'ls'";
	line->tks[1] = NULL;

	char *esperado[] = {"ls", NULL};
	char **resultado = clean_tokens(line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(clean_tokens, simple_quots_in_double_quotes)
{
	t_line *line;

	line = (t_line*)malloc(sizeof(t_line));
	init_line(line);
	line->tks = (char **)malloc(sizeof(char **) * 2);
	line->tks[0] = "\"'ls'\"";
	line->tks[1] = NULL;

	char *esperado[] = {"'ls'", NULL};
	char **resultado = clean_tokens(line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(clean_tokens, double_quots_in_simple_quotes)
{
	t_line *line;

	line = (t_line*)malloc(sizeof(t_line));
	init_line(line);
	line->tks = (char **)malloc(sizeof(char **) * 2);
	line->tks[0] = "'\"ls\"'";
	line->tks[1] = NULL;

	char *esperado[] = {"\"ls\"", NULL};
	char **resultado = clean_tokens(line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(clean_tokens, simple_quotes_in_words)
{
	t_line *line;

	line = (t_line*)malloc(sizeof(t_line));
	init_line(line);
	line->tks = (char **)malloc(sizeof(char **) * 2);
	line->tks[0] = "'l's";
	line->tks[1] = NULL;

	char *esperado[] = {"ls", NULL};
	char **resultado = clean_tokens(line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(clean_tokens, simple_quotes_in_words_and_double_quotes)
{
	t_line *line;

	line = (t_line*)malloc(sizeof(t_line));
	init_line(line);
	line->tks = (char **)malloc(sizeof(char **) * 2);
	line->tks[0] = "'l'\"s\"";
	line->tks[1] = NULL;

	char *esperado[] = {"ls", NULL};
	char **resultado = clean_tokens(line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}


TEST(clean_tokens, double_quotes_in_words)
{
	t_line *line;

	line = (t_line*)malloc(sizeof(t_line));
	init_line(line);
	line->tks = (char **)malloc(sizeof(char **) * 2);
	line->tks[0] = "\"l\"s";
	line->tks[1] = NULL;

	char *esperado[] = {"ls", NULL};
	char **resultado = clean_tokens(line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}

TEST(clean_tokens, double_quotes_in_words_and_in_double_quots)
{
	t_line *line;

	line = (t_line*)malloc(sizeof(t_line));
	init_line(line);
	line->tks = (char **)malloc(sizeof(char **) * 2);
	line->tks[0] = "\"\"l\"s\"";
	line->tks[1] = NULL;

	char *esperado[] = {"ls", NULL};
	char **resultado = clean_tokens(line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}