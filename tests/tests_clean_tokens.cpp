#include <gtest/gtest.h>
#include "../src/tokens/clean_tokens.c"
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
/* °° explodir aspas
** achar o path
** abrir o pipe
** abrir o fork
** e executar o comando
 */