# Dependências — minishell

> Gerado pelo **Scout** (Reversa) em 2026-11-11
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA

O projeto não usa gerenciador de pacotes. As dependências vêm do sistema ou do próprio repositório.

## 1. Runtime / link

| Dependência | Versão | Origem | Uso |
|-------------|--------|--------|-----|
| GNU Readline (`libreadline`) | Não fixada 🔴 | Sistema (`-lreadline` no `Makefile`) | `readline`, `add_history`, `rl_on_new_line`, `rl_replace_line`, `rl_redisplay` 🟢 |
| libc POSIX | Do sistema | Sistema | `fork`, `execve`, `pipe`, `dup`/`dup2`, `waitpid`, `signal`, `open`/`close`, `fstat`, `getcwd`, `chdir`, `access` 🟡 (lista exata a confirmar no Arqueólogo) |
| Libft | Interna | `Libft/` → `libft.a` | Strings, memória, `ft_split`, `ft_itoa`, `ft_strcmp`, `ft_free_arr` etc. 🟢 |

Headers incluídos em `includes/minishell.h`: `stdio.h`, `readline/readline.h`, `readline/history.h`, `sys/types.h`, `sys/wait.h`, `signal.h`, `sys/stat.h`, `fcntl.h`, `stdlib.h`. O `libft.h` inclui `stdlib.h` e `unistd.h` 🟢.

## 2. Build

| Ferramenta | Versão | Fonte |
|------------|--------|-------|
| `clang` | Não fixada | `Makefile`, `Libft/Makefile` 🟢 |
| GNU Make | Não fixada | `Makefile` (usa `VPATH`, `addprefix`) 🟢 |
| `ar` | — | `Libft/Makefile` (`ar -rcs`) 🟢 |

Flags: `-Wall -Werror -Wextra -g` na aplicação e `-g3` na Libft 🟢.

## 3. Testes e desenvolvimento

| Ferramenta | Versão | Fonte |
|------------|--------|-------|
| CMake | ≥ 3.15 | `tests/CMakeLists.txt` 🟢 |
| GoogleTest | Não fixada | `find_package(GTest REQUIRED)` 🟢 |
| pthread | Do sistema | Link do `runTests` 🟢 |
| Valgrind | Não fixada | Alvo `make val` (precisa de `readline.supp`, que não está no repo 🔴) |
| gdb | — | Alvo `make gdb`, que gera `a.out` 🟢 |

## 4. Módulos da Libft

- **Obrigatórios (34 + extras):** `ft_mem*`, `ft_str*`, `ft_is*`, `ft_to*`, `ft_calloc`, `ft_strdup`, `ft_substr`, `ft_strjoin`, `ft_strtrim`, `ft_split`, `ft_itoa`, `ft_strmapi`, `ft_put*_fd` 🟢
- **Extras do projeto:** `ft_arrcmp`, `ft_free_arr`, `ft_strcmp` 🟢
- **Bônus (`ft_lst*`):** só são compilados com `make bonus` na Libft. O `Makefile` principal chama apenas `make`, então eles **não entram** no `libft.a` usado pelo minishell 🟢. O minishell usa a própria `t_pipe_list` em vez de `t_list` 🟡.
