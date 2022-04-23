#include <gtest/gtest.h>
#include "../src/exec/path_finder.c"
#include "../Libft/ft_split.c"
#include "../Libft/ft_strjoin.c"
#include "../Libft/ft_strlcat.c"
#include <stdio.h>

TEST(path_finder, return_bin_of_ls)
{
	t_line	line;
	line.envp = (char **)malloc(sizeof(char **) * 2);
	line.envp[0] = ft_strdup("PATH=/usr/bin");
	line.envp[1] = NULL;
	line.cmds = (char **)malloc(sizeof(char **) * 2);
	line.cmds[0] = ft_strdup("ls");
	line.cmds[1] = NULL;
	char *esperado = "/usr/bin/ls";
	char *resultado = path_finder(&line);

	printf("resultado: %s", resultado);
	ASSERT_STREQ(resultado, esperado);
}

