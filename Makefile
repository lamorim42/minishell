#Name
NAME	= minishell
CC		= clang
CFLAGS	= -Wall -Werror -Wextra -g

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
			list_generation_bin.c \
			print_table.c \
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
			cd.c \
			env.c \
			export.c \
			unset.c \
			exit.c \
			here_doc.c \
			here_doc_utils.c \
			fd_handler.c \
			builtins_utils.c \
			exec_utils.c \
			exec_list.c \
			expand_var.c \
			expand_var_utils.c \
			fd_handler_utils.c \
			facade.c \
			tokenizer_utils.c \
			signals_utils.c \

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
			@mkdir -p $(OBJ_DIR)

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

#Rust (crates/): mesmos nomes da versão em C, com o prefixo rust-
# Os alvos rust-* não incluem a GUI (msh-gui), que precisa das bibliotecas
# do GTK; ela tem os alvos rust-gui-*.
CARGO	= cargo
CRATES	= -p msh-syntax -p msh-runtime -p msh-cli -p msh-files
CORE	= --workspace --exclude msh-gui

rust:
			$(CARGO) build $(CORE)

rust-run:
			$(CARGO) run -p msh-cli

rust-deb:
			$(CARGO) deb -p msh-cli

rust-release:
			$(CARGO) build $(CORE) --release

rust-test:
			$(CARGO) test $(CORE)

rust-fmt:
			$(CARGO) fmt --all

rust-lint:
			$(CARGO) fmt --all --check
			$(CARGO) clippy $(CORE) --all-targets -- -D warnings

# O mesmo que o CI roda: lint + testes, com o Cargo.lock travado.
rust-check:	rust-lint
			$(CARGO) test $(CORE) --locked

# Fase 2: Miau Files (GTK4 + libadwaita + VTE).
rust-gui:
			$(CARGO) build -p msh-gui -p msh-cli -p msh-files

rust-gui-run:	rust-gui
			$(CARGO) run -p msh-gui

# Pacote miau-files (GUI + helper do polkit). Instale junto com o minishell:
# sudo apt install ./target/debian/minishell_*.deb ./target/debian/miau-files_*.deb
rust-gui-deb:
			$(CARGO) build --release -p msh-files -p msh-cli
			$(CARGO) deb -p msh-cli
			$(CARGO) deb -p msh-gui

# Lint e testes da GUI, mais o autoteste com a janela de verdade (precisa de
# uma tela: rode dentro da sessão gráfica ou com xvfb-run).
rust-gui-check:	rust-gui
			$(CARGO) clippy -p msh-gui -p msh-files --all-targets -- -D warnings
			$(CARGO) test -p msh-gui -p msh-files --locked
			./target/debug/miau-files --self-test

# Apaga só o que é nosso; as dependências já compiladas ficam.
rust-clean:
			$(CARGO) clean $(CRATES) -p msh-gui

# Apaga a pasta target/ inteira.
rust-fclean:
			$(CARGO) clean

rust-re:	rust-fclean rust

.PHONY: rust rust-run rust-deb rust-release rust-test rust-fmt rust-lint rust-check rust-gui rust-gui-run rust-gui-deb rust-gui-check rust-clean rust-fclean rust-re
