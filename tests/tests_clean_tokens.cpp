#include <gtest/gtest.h>
#include "../src/tokens/clean_tokens.c"
#include "../Libft/ft_strtrim.c"
#include <stdio.h>


TEST(clean_tokens, delete_quot)
{
	t_line *line;

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

	line->tks = (char **)malloc(sizeof(char **) * 2);
	line->tks[0] = "ls";
	line->tks[1] = NULL;

	char *esperado[] = {"ls", NULL};
	char **resultado = clean_tokens(line);

	EXPECT_TRUE(ft_arrcmp(esperado, resultado) == 0);
}
/* °° explodir aspas
** achar o path
** abrir o pipe
** abrir o fork
** e executar o comando
 */