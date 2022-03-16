#Name
NAME	= minishell

#

# Source
SRC	=	main.c \
		lists.c \
		minishell.c \

OBJTS	= $(SRC:.c=.o)
OBJ		= $(addprefix ./obj/, $(OBJTS))

VPATH	= ./src/ ./lists/

#Git config
SEP ="\n\e[0;36m--------------------------------------------------------\e[0m\n"
ADD	= .


obj/%.o: %.c


git:
		git status
		@echo $(SEP)
		git add $(ADD)
		@echo $(SEP)
		git commit
		@echo $(SEP)
		git status