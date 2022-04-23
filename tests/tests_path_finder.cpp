#include <gtest/gtest.h>
#include "../src/exec/path_finder.c"
#include <stdio.h>

TEST(path_finder, return_bin_of_ls)
{
	char *line[] = {"ls", NULL};
	char *esperado[] = {"/usr/bin/ls", NULL};
	char **resultado = path_finder(line);

	EXPECT_TRUE(ft_arrcmp(resultado, esperado) == 0);
}

