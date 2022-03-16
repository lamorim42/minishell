#Name
NAME	= minishell
CC		= gcc
CFLAGS	= -Wall -Werror -Wextra -g


# Source
SRC		=	main.c \

OBJTS	= $(SRC:.c=.o)
OBJ		= $(addprefix ./obj/, $(OBJTS))
OBJ_DIR	= ./obj

RM		= rm -rf

VPATH	= ./src/ ./lists/

#Git config
SEP		="\n\e[0;36m--------------------------------------------------------\e[0m\n"
ADD		= .

#Rules
$(OBJ_DIR)/%.o: %.c
			$(CC) $(CFLAGS) -c $< -o $@

all:		obj_dir $(NAME)

$(NAME):	$(OBJ)
			$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

obj_dir:
		mkdir -p $(OBJ_DIR)

clean:
		$(RM) $(OBJ)


fclean:	clean
		$(RM) $(NAME)
		$(RM) obj/

re:		fclean all

git:
		git status
		@echo $(SEP)
		git add $(ADD)
		@echo $(SEP)
		git commit
		@echo $(SEP)
		git status

.PHONY= all clean fclean re $(NAME)