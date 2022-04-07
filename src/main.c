/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 08:29:06 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/06 10:18:26 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	init_line(t_line *line, int argc, char **argv, char **envp);
static void	count_pipe(t_line *line);
//void	arr_map(t_line *line, char *(*f)(char const *s1, char const *set));

int	main(int argc, char **argv, char **envp)
{
	t_line	line;
	int	i;

	i = 0;
	/* if (argc != 1)
		printf("Use ./minishell"); */
	init_line(&line, argc, argv, envp);
	line.pipeline = readline("miau> ");
	while (1)
	{
		if (ft_strlen(line.pipeline))
		{
			count_pipe(&line);
			line.tks = get_tokens(line.pipeline);
			line.lex = lexical_analysis(line.tks);
			syntax_analisys(line.lex);
			// expansão
			create_cmd_arr(&line);
		//	arr_map(&line, ft_strtrim);
			create_cmd_table(&line);
			split_path(&line);
			line.bin = check_path(&line);
			exec_cmd_pipe(&line);
			//ft_fork(&line);
			free(line.pipeline);
			ft_free_array(line.bin);
			ft_free_array(line.cmds_table);
			ft_free_array(line.path);
			ft_free_array(line.tks);
			ft_free_array(line.lex);
			ft_free_arrcmds(line.array_cmds);
		}
	}
	return (0);
}

static void	init_line(t_line *line, int argc, char **argv, char **envp)
{
	line->pipeline = NULL;
	line->tks = NULL;
	line->lex = NULL;
	line->nb_pipes = 0;
	line->nb_cmds = 0;
	line->array_cmds = NULL;
	line->argv = argv;
	line->envp = envp;
	line->argc = argc;
	line->path = NULL;
	line->bin = NULL;
}

static void	count_pipe(t_line *line)
{
	char	*temp;

	temp = line->pipeline;
	while (*temp)
	{
		if (*temp == '|')
			line->nb_pipes++;
		temp++;
	}
}

/* void	arr_map(t_line *line, char *(*f)(char const *s1, char const *set))
{
	int	len;
	int	i;

	i = 0;
	len = 0;
	while (line->array_cmds[len])
		len++;
	line->map_cmds = malloc(sizeof(char *) * (len + 1));
	if(line->map_cmds == NULL)
		return_error();
	while(line->array_cmds[i])
	{
		line->map_cmds[i] = (*f)(line->array_cmds[i][0], " ");
		i++;
	}
} */

void return_error(void)
{
	printf("error!\n");
}
