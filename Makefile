#Name
NAME	= minishell
CC		= clang
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
			creat_cmd_list.c \
			list_commands.c \


LIBFT_PATH = ./Libft
LIBFT = $(LIBFT_PATH)/libft.a

OBJTS	= $(SRC:.c=.o)
OBJ		= $(addprefix ./obj/, $(OBJTS))
OBJ_DIR	= ./obj

OBJTS_TESTS	= $(SRC_TEST:.c=.o)
OBJ_TESTS		= $(addprefix ./obj/, $(OBJTS_TESTS))

RM		= rm -rf

INCLUDE	= -I includes -I $(LIBFT_PATH)

VPATH	= ./src/ ./src/tokens/ ./src/exec

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

test:	obj_dir $(OBJ_TESTS) $(OBJ) $(LIBFT)
		$(CC) $(CFLAGS) $(INCLUDE) $(OBJ_TESTS) $(OBJ) $(LIBFT) -o tests_minishell -lcriterion

gdb:	obj_dir $(OBJ) $(LIBFT)
		$(CC) $(CFLAGS) $(INCLUDE) $(OBJ) $(LIBFT) -o a.out

.PHONY= all clean fclean re $(NAME)