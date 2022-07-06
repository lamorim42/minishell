#Name
NAME	= minishell
CC		= gcc
CFLAGS	= -Wall -Werror -Wextra -g3

#Source
SRC		=	main.c \
			count_tks.c \
			tokenizer.c \
			lexical_analyzer.c \
			sintax_analysis.c \
			clean_tokens.c \
			path_finder.c \
			init.c \
			exec_path.c \
			init_fork.c \
			signals.c \
			make_list.c \
			creat_cmd_table.c \
			list_commands.c \
			init_hash.c \
			create_node.c \
			create_table.c \
			free_hash_table.c \
			hash_function.c \
			insert_node.c \
			search_item.c \
			delete_node.c \
			echo.c \
			pwd.c \
			free.c \
			arr_utils.c \
			cmd_table_utils.c \
			cd.c \
			env.c \
			export.c \
			unset.c \
			exit.c \
			here_doc.c \


LIBFT_PATH = ./Libft
LIBFT = $(LIBFT_PATH)/libft.a

OBJTS	= $(SRC:.c=.o)
OBJ		= $(addprefix ./obj/, $(OBJTS))
OBJ_DIR	= ./obj

OBJTS_TESTS	= $(SRC_TEST:.c=.o)
OBJ_TESTS		= $(addprefix ./obj/, $(OBJTS_TESTS))

RM		= rm -rf

INCLUDE	= -I includes -I $(LIBFT_PATH)

VPATH	= ./src/ ./src/tokens/ ./src/exec ./src/hash_table/ ./src/builtins/

#Git config
SEP		="\n\e[0;36m--------------------------------------------------------\e[0m\n"
add		= .

#Rules
$(OBJ_DIR)/%.o: %.c
			$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

all:		obj_dir $(NAME)

$(NAME):	$(OBJ) $(LIBFT)
			$(CC) $(CFLAGS) $(INCLUDE) $(OBJ) $(LIBFT) -o $(NAME) -lreadline

obj_dir:
			mkdir -p $(OBJ_DIR)

$(LIBFT):
			make -C $(LIBFT_PATH)

clean:
		$(RM) $(OBJ)
		$(RM) $(OBJ_TESTS)
		make clean -C $(LIBFT_PATH)


fclean:	clean
		$(RM) $(NAME)
		$(RM) tests_minishell
		$(RM) obj/
		make fclean -C $(LIBFT_PATH)

re:		fclean all

git:	fclean
		git status
		@echo $(SEP)
		git add $(add)
		@echo $(SEP)
		git commit
		@echo $(SEP)
		git status

run:
	./minishell

val:
	valgrind --suppressions=readline.supp -s --show-leak-kinds=all --leak-check=full --track-fds=yes ./minishell

gdb:	obj_dir $(OBJ) $(LIBFT)
		$(CC) $(CFLAGS) $(INCLUDE) $(OBJ) $(LIBFT) -o a.out

.PHONY= all clean fclean re $(NAME)