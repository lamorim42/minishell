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

## 6. Reescrita em Rust (`crates/`)

> Adicionado em 2026-11-22, após a unificação com a `feature/refac-rust` (PR #36). Critério da coluna **Divergências**: pela decisão L-08 (`questions.md`, Pergunta 9), o Rust segue o POSIX nos itens ⚠️ DIVERGÊNCIA / `@divergente` da unit e não reproduz os bugs do C.
> Legenda do marco: ✅ pronto · ⏳ planejado.

| Arquivo em Rust | Unit correspondente | Equivalente em C | Marco | Divergências |
|-----------------|---------------------|------------------|-------|--------------|
| `crates/msh-syntax/src/lexer.rs`, `token.rs` | `analisar-linha-de-comando/` | `src/tokens/count_tks.c`, `tokenizer.c`, `tokenizer_utils.c`, `lexical_analyzer.c`, `clean_tokens.c` | M1 ✅ | POSIX 2.2 e 2.3 |
| `crates/msh-syntax/src/parser.rs`, `ast.rs` | `analisar-linha-de-comando/` | `src/tokens/sintax_analysis.c`, `src/exec/make_list.c` | M1 ✅ | POSIX 2.10 (subconjunto) |
| `crates/msh-syntax/src/error.rs` | `analisar-linha-de-comando/` (mensagens de erro de sintaxe) | `src/tokens/sintax_analysis.c`, `src/free.c` (`free_sintax`) | M1 ✅ | mensagens no formato do bash |
| `crates/msh-syntax/src/display.rs` | n/a (forma canônica para testes) | — | M1 ✅ | n/a |
| `crates/msh-runtime/src/shell.rs` | `iniciar-sessao-e-ler-comando/`, `tratar-sinais/` (estado global) | `g_minishell` em `includes/minishell.h`, `src/init.c` | M2 ✅ (parcial) | estado sem variável global |
| `crates/msh-runtime/src/env.rs` | `gerenciar-variaveis-de-ambiente/` | `src/hash_table/*.c` | M2 ✅ | POSIX |
| `crates/msh-runtime/src/expand.rs` | `expandir-variaveis/` | `src/exec/expand_var.c`, `expand_var_utils.c`, `exec_utils.c` | M2 ✅ | POSIX 2.6 (til, parâmetros, *field splitting*, remoção de aspas) |
| `crates/msh-runtime/src/builtins/env.rs` | `gerenciar-variaveis-de-ambiente/` | `src/builtins/env.c`, `src/hash_table/print_table.c` | M2 ✅ | POSIX |
| `crates/msh-runtime/src/builtins/export.rs` | `gerenciar-variaveis-de-ambiente/` | `src/builtins/export.c`, `builtins_utils.c` | M2 ✅ | POSIX (`export -p` no formato do `bash --posix`) |
| `crates/msh-runtime/src/builtins/unset.rs` | `gerenciar-variaveis-de-ambiente/` | `src/builtins/unset.c` | M2 ✅ | POSIX |
| `crates/msh-runtime/src/builtins/mod.rs` | `executar-comando-simples/` (despacho de builtins) | `src/builtins/builtins_utils.c` (`exec_builtins`, `is_a_builtin`) | M2 ✅ (parcial) | POSIX |
| `crates/msh-runtime/src/exec/mod.rs` | `executar-comando-simples/` | `src/facade.c`, `src/exec/exec_list.c`, `init_fork.c` | M3 ✅ | POSIX 2.9.1 (atribuições, special built-ins, 2.8.1): `export` chega aos filhos (BUG-03), `pwdx` não é builtin (BUG-04) |
| `crates/msh-runtime/src/exec/path.rs` | `executar-comando-simples/` | `src/exec/path_finder.c`, `exec_path.c`, `list_generation_bin.c` | M3 ✅ | XBD 8.3: arquivo sem permissão no `PATH` não conta (127, como no dash); `execve` falho dá 126 ou 127 conforme o erro (BUG-16); `PATH` ausente usa o padrão do dash |
| `crates/msh-runtime/src/exec/sys.rs` | `executar-comando-simples/`, `tratar-sinais/` | `src/exec/init_fork.c`, `src/signals*.c` | M3 ✅ | único `unsafe` (ADR-005); `$?` = 128+n quando morto por sinal |
| `crates/msh-runtime/src/builtins/echo.rs` | `imprimir-texto-echo/` | `src/builtins/echo.c` | M3 ✅ | segue o dash onde o POSIX deixa aberto: só o 1º `-n` é opção (`-nn` é texto, EC-04 coincide), escapes do XSI; `echo a "" b` dá `a  b` (EC-07 corrigido) |
| `crates/msh-runtime/src/builtins/cd.rs`, `pwd.rs` | `navegar-diretorios/` | `src/builtins/cd.c`, `pwd.c` | M3 ✅ | POSIX `cd`/`pwd`: `cd -`, `CDPATH`, `-L`/`-P`, `PWD`/`OLDPWD`; `HOME` ausente dá erro e status 1 (BUG-12 corrigido); `pwd` usa `getcwd` se `PWD` for inválido |
| `crates/msh-runtime/src/builtins/exit.rs` | `encerrar-shell/` | `src/builtins/exit.c` | M3 ✅ | `exit` sem argumento usa `$?` (XT-01 corrigido); `exit 2 3` dá `too many arguments` (BUG-21 corrigido); negativo é erro, como no dash |
| `msh-runtime` (pipes, redirecionamentos, heredoc) | `executar-pipeline/`, `redirecionar-entrada-e-saida/`, `ler-heredoc/` | `src/exec/*.c`, `src/fd_handler*.c` | M4 ⏳ | POSIX 2.7, 2.9.2; builtin com redirecionamento roda no pai (RE-01) |
| `crates/msh-cli/src/main.rs`, `input.rs` | `iniciar-sessao-e-ler-comando/`, `encerrar-shell/` (Ctrl+D) | `src/main.c` | M3 ✅ | POSIX `sh`: interativo só com terminal; `-c`, script e stdin lido sem consumir além da linha; Ctrl+D sai com `$?` (cenário divergente corrigido); aspas abertas continuam na linha seguinte |
| `msh-cli` (sinais, OSC 7) | `tratar-sinais/` | `src/signals*.c` | M5 ⏳ | ADR-008 (sem estado global) |

### Testes em Rust

| Arquivo | Unit | Observação |
|---------|------|------------|
| `crates/msh-syntax/tests/lexer.rs`, `parser.rs`, `properties.rs`, `snapshots/` | `analisar-linha-de-comando/` | inclui casos portados dos testes GTest (`tests/tests_*.cpp`, ADR-002 desta extração) |
| `crates/msh-runtime/tests/expand.rs` | `expandir-variaveis/` | |
| `crates/msh-runtime/tests/builtins.rs` | `gerenciar-variaveis-de-ambiente/`, `imprimir-texto-echo/`, `encerrar-shell/` | |
| `crates/msh-cli/tests/compat_m3.rs` (+ `common/mod.rs`) | `executar-comando-simples/`, `imprimir-texto-echo/`, `navegar-diretorios/`, `encerrar-shell/` | compara com o dash e o `bash --posix`; cobre os cenários `@divergente` (TT-04 e TT-02 das units) |
| `crates/msh-cli/tests/modes.rs` | `iniciar-sessao-e-ler-comando/` | `-c`, script e stdin |
