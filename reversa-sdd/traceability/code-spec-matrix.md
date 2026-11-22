# Code/Spec Matrix

> Redator (Reversa) · 2026-11-22
> Leitura: para cada arquivo do legado, a unit que especifica o seu comportamento. 🟢 = coberto e validado no código (e, na maior parte, observado em execução); 🟡 = coberto em parte ou só indiretamente; n/a = sem unit (candidato a análise adicional ou fora do escopo funcional).

## 1. Código do shell (`src/`, `includes/`)

| Arquivo do legado | Unit correspondente | Cobertura |
|-------------------|---------------------|-----------|
| `src/main.c` | `iniciar-sessao-e-ler-comando/` (laço, EOF, comentário); `redirecionar-entrada-e-saida/` (`file_exists`) | 🟢 |
| `src/init.c` | `iniciar-sessao-e-ler-comando/` | 🟢 |
| `src/facade.c` | `analisar-linha-de-comando/`, `expandir-variaveis/`, `ler-heredoc/`, `executar-comando-simples/` (orquestração `exec_pipe_line`, `alloc_commands`) | 🟢 |
| `src/free.c` | `analisar-linha-de-comando/` (`free_sintax`); `executar-comando-simples/` (`error_msg`, `free_exit`, `free_list`, `free_line`) | 🟢 |
| `src/arr_utils.c` | `executar-comando-simples/` (`copy_array`); `gerenciar-variaveis-de-ambiente/` (`ft_array_len`). `print_array` é depuração, sem uso no fluxo | 🟡 |
| `src/fd_handler.c` | `redirecionar-entrada-e-saida/`, `executar-pipeline/` | 🟢 |
| `src/fd_handler_utils.c` | `redirecionar-entrada-e-saida/` (`open_fds`, `check_valid_fd`), `executar-pipeline/` (ramo `PIPE`), `iniciar-sessao-e-ler-comando/` (`close_std_fd`) | 🟢 |
| `src/signals.c` | `tratar-sinais/` | 🟢 |
| `src/signals_utils.c` | `tratar-sinais/`, `ler-heredoc/` (`sig_here`) | 🟢 |
| `src/tokens/count_tks.c` | `analisar-linha-de-comando/` | 🟢 |
| `src/tokens/tokenizer.c` | `analisar-linha-de-comando/` | 🟢 |
| `src/tokens/tokenizer_utils.c` | `analisar-linha-de-comando/` (`building_tokens`, `ft_strcmp_len`) | 🟢 |
| `src/tokens/lexical_analyzer.c` | `analisar-linha-de-comando/` | 🟢 |
| `src/tokens/sintax_analysis.c` | `analisar-linha-de-comando/` (`sintax_analysis`); `iniciar-sessao-e-ler-comando/` (`verify_quots`) | 🟢 |
| `src/tokens/clean_tokens.c` | `analisar-linha-de-comando/` | 🟢 |
| `src/exec/expand_var.c` | `expandir-variaveis/` | 🟢 |
| `src/exec/expand_var_utils.c` | `expandir-variaveis/` | 🟢 |
| `src/exec/exec_utils.c` | `expandir-variaveis/` (`take_prefix_var`, `sufix_var`); `executar-comando-simples/` (`is_command`) | 🟢 |
| `src/exec/creat_cmd_table.c` | `executar-comando-simples/` (`creat_list_cmd`, `make_args`, `get_arg_size`); `ler-heredoc/` (`char_cat`) | 🟢 |
| `src/exec/make_list.c` | `executar-comando-simples/` | 🟢 |
| `src/exec/list_generation_bin.c` | `executar-comando-simples/` | 🟢 |
| `src/exec/path_finder.c` | `executar-comando-simples/` | 🟢 |
| `src/exec/exec_list.c` | `executar-comando-simples/`, `executar-pipeline/`, `expandir-variaveis/` (`update_status_code` → `$?`) | 🟢 |
| `src/exec/init_fork.c` | `executar-comando-simples/`, `executar-pipeline/`, `tratar-sinais/` | 🟢 |
| `src/exec/exec_path.c` | `executar-comando-simples/`, `redirecionar-entrada-e-saida/` (`make_redirections`) | 🟢 |
| `src/exec/here_doc.c` | `ler-heredoc/` | 🟢 |
| `src/exec/here_doc_utils.c` | `ler-heredoc/` | 🟢 |
| `src/builtins/builtins_utils.c` | `executar-comando-simples/` (`exec_builtins`, `is_a_builtin`); `gerenciar-variaveis-de-ambiente/` (`is_args_ok`, `insert_var_in_hash`, `insert_simple_var_in_hash`) | 🟢 |
| `src/builtins/echo.c` | `imprimir-texto-echo/` | 🟢 |
| `src/builtins/cd.c` | `navegar-diretorios/`; `gerenciar-variaveis-de-ambiente/` (`update_var_env`) | 🟢 |
| `src/builtins/pwd.c` | `navegar-diretorios/` | 🟢 |
| `src/builtins/export.c` | `gerenciar-variaveis-de-ambiente/` | 🟢 |
| `src/builtins/unset.c` | `gerenciar-variaveis-de-ambiente/` | 🟢 |
| `src/builtins/env.c` | `gerenciar-variaveis-de-ambiente/` | 🟢 |
| `src/builtins/exit.c` | `encerrar-shell/` | 🟢 |
| `src/hash_table/create_table.c` | `gerenciar-variaveis-de-ambiente/`, `iniciar-sessao-e-ler-comando/` (`population_hash_table`) | 🟢 |
| `src/hash_table/create_node.c` | `gerenciar-variaveis-de-ambiente/` | 🟢 |
| `src/hash_table/insert_node.c` | `gerenciar-variaveis-de-ambiente/` | 🟢 |
| `src/hash_table/delete_node.c` | `gerenciar-variaveis-de-ambiente/` | 🟢 |
| `src/hash_table/search_item.c` | `gerenciar-variaveis-de-ambiente/`, `expandir-variaveis/` | 🟢 |
| `src/hash_table/hash_function.c` | `gerenciar-variaveis-de-ambiente/` | 🟢 |
| `src/hash_table/print_table.c` | `gerenciar-variaveis-de-ambiente/` (`env`) | 🟢 |
| `src/hash_table/free_hash_table.c` | `gerenciar-variaveis-de-ambiente/`, `encerrar-shell/` | 🟢 |
| `includes/minishell.h` | `analisar-linha-de-comando/` (`t_line`), `executar-comando-simples/` (`t_pipe_list`), `gerenciar-variaveis-de-ambiente/` (`t_hash_table`, `SIZE_TABLE`), `tratar-sinais/` (`g_minishell`) | 🟢 |

## 2. Libft (`Libft/`)

| Arquivo do legado | Unit correspondente | Cobertura |
|-------------------|---------------------|-----------|
| `Libft/ft_strncmp.c` | `executar-comando-simples/` (semântica de prefixo, causa de BUG-04 e BUG-20); usado em quase todas as units | 🟢 |
| `Libft/ft_strchr.c`, `ft_strlen.c`, `ft_strlcpy.c`, `ft_strdup.c`, `ft_strtrim.c` | `analisar-linha-de-comando/` | 🟢 |
| `Libft/ft_substr.c`, `ft_strjoin.c` | `expandir-variaveis/`, `ler-heredoc/` | 🟢 |
| `Libft/ft_split.c` | `gerenciar-variaveis-de-ambiente/` (BUG-06), `executar-comando-simples/` (`PATH`) | 🟢 |
| `Libft/ft_isalpha.c` | `gerenciar-variaveis-de-ambiente/` (validação de identificador) | 🟢 |
| `Libft/ft_isdigit.c`, `ft_atoi.c` | `encerrar-shell/` | 🟢 |
| `Libft/ft_itoa.c` | `expandir-variaveis/` (`$?`) | 🟢 |
| `Libft/ft_putstr_fd.c` | `executar-comando-simples/` (`error_msg`) | 🟢 |
| `Libft/ft_calloc.c`, `ft_bzero.c`, `ft_free_arr.c` | transversal (alocação e liberação) | 🟡 |
| `Libft/ft_arrcmp.c` | só nos testes GTest (`analisar-linha-de-comando/`, TT-01) | 🟡 |
| `Libft/libft.h`, `Libft/Makefile` | n/a (build) | n/a |
| Demais 28 arquivos (`ft_lst*`, `ft_mem*`, `ft_is{alnum,ascii,print}`, `ft_put{char,endl,nbr}_fd`, `ft_strcmp`, `ft_strlcat`, `ft_strmapi`, `ft_strnstr`, `ft_strrchr`, `ft_to{lower,upper}`) | n/a (não usados pelo shell) | n/a |

## 3. Testes e build

| Arquivo do legado | Unit correspondente | Cobertura |
|-------------------|---------------------|-----------|
| `tests/tests_tokenizer.cpp`, `tests_count_tks.cpp`, `tests_lexical_analyzer.cpp`, `tests_sintax_analysis.cpp`, `tests_clean_tokens.cpp` | `analisar-linha-de-comando/` (critérios de pronto T-02 a T-09) | 🟢 |
| `tests/CMakeLists.txt` | n/a (build dos testes, ADR-002) | n/a |
| `Makefile` | n/a (build, ADR-010) | n/a |

## 4. Resumo de cobertura

| Grupo | Arquivos | Com unit (🟢/🟡) | n/a |
|-------|----------|------------------|-----|
| `src/` + `includes/` | 44 | 44 (43 🟢, 1 🟡) | 0 |
| `Libft/` | 48 | 19 (16 🟢, 3 🟡) | 29 |
| `tests/` + `Makefile` | 7 | 5 🟢 | 2 |
| **Total** | **99** | **68** | **31** |

- Cobertura do código funcional do shell (`src/` + `includes/`): **100%**
- Cobertura do repositório inteiro: **68,7%** (68/99); os 31 arquivos n/a são build ou funções da Libft que o shell não chama

## 5. Achados novos do Redator (não constam em `code-analysis.md` §9)

| ID | Resumo | Arquivo | Unit |
|----|--------|---------|------|
| BUG-20 | `ft_strncmp` retorna 0 quando uma das strings termina: comando vazio ou prefixo de `PIPE`/`REDO`/`REDA`/`REDI`/`HERE` é ignorado em silêncio; prefixo de builtin vira `command not found` | `Libft/ft_strncmp.c`, `src/exec/exec_utils.c:15-22` | `executar-comando-simples/` |
| BUG-21 | `exit 2 <arg>` encerra com 2 em vez de `too many arguments` | `src/builtins/exit.c:51` | `encerrar-shell/` |
| RX-06 (confirmado) | Valor auto-referente (`A='$A'`) trava a expansão em loop infinito; antes era 🟡 | `src/exec/expand_var.c:27-28` | `expandir-variaveis/` |
