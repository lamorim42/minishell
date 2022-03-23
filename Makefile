#Name
NAME	= minishell
CC		= clang
CFLAGS	= -Wall -Werror -Wextra -g


#Source
SRC		=	main.c \
			count_words.c \
			get_tokens.c \
			lex.c \
			gramatic.c \
			array.c \

LIBFT_PATH = ./Libft
LIBFT = $(LIBFT_PATH)/libft.a

OBJTS	= $(SRC:.c=.o)
OBJ		= $(addprefix ./obj/, $(OBJTS))
OBJ_DIR	= ./obj

RM		= rm -rf

INCLUDE	= -I includes -I $(LIBFT_PATH)

VPATH	= ./src/ ./src/tokenizer/ ./src/parsing/

#Git config
SEP		="\n\e[0;36m--------------------------------------------------------\e[0m\n"
ADD		= .

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
		make clean -C $(LIBFT_PATH)


fclean:	clean
		$(RM) $(NAME)
		$(RM) obj/
		make fclean -C $(LIBFT_PATH)

re:		fclean all

git:	fclean
		git status
		@echo $(SEP)
		git add $(ADD)
		@echo $(SEP)
		git commit
		@echo $(SEP)
		git status

.PHONY= all clean fclean re $(NAME)