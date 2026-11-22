# Inventário — minishell (MiauShell)

> Gerado pelo **Scout** (Reversa) em 2026-11-11
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA

## 1. Visão geral

| Item | Valor |
|------|-------|
| Nome | `minishell`, apelidado de **MiauShell** (README) 🟢 |
| Tipo | Interpretador de comandos interativo (CLI), um clone parcial do bash feito no currículo da 42 São Paulo 🟢 |
| Autores | Daniela Monteiro (`dmonteir`) e Luís Amorim (`lamorim`) 🟢 |
| Período ativo | De 2022-03-16 a 2022-07-23 pelos headers e pelo git. O commit de 2026-11-11 é só o `Reversa Init` 🟢 |
| Commits | 424 🟢 |
| Linguagem principal | C (padrão da 42, sem padrão ISO declarado no Makefile) 🟢 |
| Compilador | `clang`, flags `-Wall -Werror -Wextra -g` 🟢 |
| Dependência externa | GNU Readline (`-lreadline`) 🟢 |
| Prompt | `miau> ` 🟢 |
| Arquitetura declarada | Pipe-and-filter, com Facade como função núcleo (README) 🟢 |
| Metodologia declarada | TDD na parte de tokenização (README) 🟢 |
| Banco de dados | Nenhum 🟢 |
| Integrações externas | Nenhuma (só syscalls POSIX e Readline) 🟢 |
| CI/CD | Nenhum (não há `.github/`, Jenkinsfile nem afins) 🟢 |
| Docker | Nenhum 🟢 |

## 2. Contagem por linguagem

| Linguagem | Extensões | Arquivos | Linhas |
|-----------|-----------|---------:|-------:|
| C (aplicação, `src/`) | `.c` | 52 | ~2.900 |
| C (Libft) | `.c` | 37 | — |
| C headers | `.h` | 2 | — |
| C++ (testes GTest) | `.cpp` | 5 | 1.146 |
| CMake | `CMakeLists.txt` | 1 | — |
| Make | `Makefile` | 2 | — |
| Markdown | `.md` | 2 | — |

No total são 5.204 linhas em `src/`, `Libft/` e `tests/`.

## 3. Árvore de diretórios

```
minishell/
├── Makefile                  # build principal (clang + libft + readline)
├── README.md                 # descrição do projeto e da arquitetura
├── .gitignore
├── assets/miaushell.gif      # demonstração
├── includes/
│   └── minishell.h           # header único: structs, macros, protótipos
├── Libft/                    # biblioteca utilitária própria da 42 (libft.a)
│   ├── Makefile
│   ├── libft.h
│   └── ft_*.c  (37 arquivos)
├── src/
│   ├── main.c                # entry point + REPL (readline loop)
│   ├── facade.c              # orquestra o pipeline por linha (Facade)
│   ├── init.c                # init_line()
│   ├── free.c                # liberação de memória e mensagens de erro
│   ├── arr_utils.c           # utilitários de array de strings
│   ├── fd_handler.c          # redirecionamentos (<, >, >>) e fds de pipe
│   ├── fd_handler_utils.c    # open_fds, close_std_fd
│   ├── signals.c             # sinais no processo pai / ignorar
│   ├── signals_utils.c       # sinais no prompt e no heredoc
│   ├── tokens/               # análise da entrada (lexer/parser)
│   │   ├── count_tks.c
│   │   ├── tokenizer.c
│   │   ├── tokenizer_utils.c
│   │   ├── lexical_analyzer.c
│   │   ├── sintax_analysis.c
│   │   └── clean_tokens.c
│   ├── exec/                 # expansão, lista de comandos, heredoc, execução
│   │   ├── expand_var.c / expand_var_utils.c
│   │   ├── creat_cmd_table.c / make_list.c
│   │   ├── here_doc.c / here_doc_utils.c
│   │   ├── list_generation_bin.c / path_finder.c
│   │   ├── exec_list.c / exec_path.c / exec_utils.c
│   │   └── init_fork.c
│   ├── builtins/             # echo, cd, pwd, export, unset, env, exit
│   └── hash_table/           # tabela hash do ambiente (variáveis)
├── tests/                    # GTest + CMake (só o módulo tokens)
│   ├── CMakeLists.txt
│   └── tests_*.cpp (5 arquivos)
└── .vscode/settings.json
```

## 4. Módulos identificados

| Módulo | Caminho | Arquivos | Responsabilidade (superficial) |
|--------|---------|---------:|--------------------------------|
| `core` | `src/main.c`, `facade.c`, `init.c`, `free.c`, `arr_utils.c` | 5 | REPL, orquestração por linha (Facade), init, liberação de memória e utilitários 🟢 |
| `tokens` | `src/tokens/` | 6 | Contagem de tokens, tokenização, análise léxica, análise sintática e remoção de aspas 🟢 |
| `exec` | `src/exec/` | 13 | Expansão de `$VAR`, montagem da lista de comandos, heredoc, resolução no `PATH`, fork/execve e wait 🟢 |
| `redirections` | `src/fd_handler.c`, `fd_handler_utils.c` | 2 | Abertura, fechamento e `dup2` de fds para `<`, `>`, `>>` e pipes 🟡 (o nome do módulo foi inferido) |
| `builtins` | `src/builtins/` | 8 | `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit` 🟢 |
| `hash_table` | `src/hash_table/` | 8 | Tabela hash com encadeamento (tamanho 149) que guarda as variáveis de ambiente 🟢 |
| `signals` | `src/signals.c`, `signals_utils.c` | 2 | SIGINT/SIGQUIT no prompt, no pai durante a execução e no heredoc 🟢 |
| `libft` | `Libft/` | 37 + header | Biblioteca de apoio (strings, memória, listas) 🟢 |

### Pipeline por linha (visão do Scout, de `src/facade.c`)

```
readline("miau> ")
  → verify_quots → is_a_comment                 (main.c)
  → building_tokens → sintax_analysis           (tokens/)
  → expand_var → clean_tokens                   (exec/ + tokens/)
  → creat_list_cmd → creat_here_doc             (exec/)
  → fork: here_doc_verification (processo filho)
  → list_generation_bin → alloc_commands → open_fds
  → exec_list (fork/execve ou builtin)
  → add_history → free_line
```
🟢 extraído de `facade.c` e `main.c`. O Arqueólogo vai detalhar cada etapa.

## 5. Pontos de entrada

| Caminho | Tipo |
|---------|------|
| `src/main.c` → `main()` | Entry point da aplicação 🟢 |
| `src/facade.c` → `facade()` | Núcleo do pipeline por linha 🟢 |
| `Makefile` (alvos `all`, `clean`, `fclean`, `re`, `run`, `val`, `gdb`, `git`) | Build e execução 🟢 |
| `tests/CMakeLists.txt` → executável `runTests` | Testes 🟢 |

## 6. Estruturas de dados centrais (`includes/minishell.h`)

| Struct | Papel |
|--------|-------|
| `t_line` | Estado da linha em processamento: string crua, tokens (`tks`, `lex`, `ctks`), fds padrão salvos, `envp`, `status_code`, pids e lista de comandos 🟢 |
| `t_pipe_list` | Nó de uma lista duplamente encadeada: `args` + union (`bin` ou `fd[2]`) 🟢 |
| `t_hash_table` / `t_hash_item` | Tabela hash do ambiente, com colisão resolvida por lista encadeada 🟢 |
| `struct s_minishell g_minishell` | **Variável global** com ponteiros para `line` e `table`, usada pelos handlers de sinal (24 referências) 🟢 |

## 7. Configuração e ambiente

- Não há `.env`, `config/` nem arquivos de settings da aplicação 🟢.
- O ambiente vem do `envp` de `main()` e é copiado para a hash table (`population_hash_table`) 🟢.
- `make val` usa `readline.supp`, que está no `.gitignore` e **não existe no repositório** 🔴.

## 8. Testes

| Item | Valor |
|------|-------|
| Framework | GoogleTest (`find_package(GTest REQUIRED)`) + CMake ≥ 3.15 🟢 |
| Arquivos | 5 (`tests_count_tks`, `tests_tokenizer`, `tests_lexical_analyzer`, `tests_sintax_analysis`, `tests_clean_tokens`) 🟢 |
| Casos (`TEST(...)`) | ~84 (32 + 22 + 13 + 7 + 10) 🟢 |
| Cobertura | Só `src/tokens/` e `src/init.c`. Não há teste para `exec`, `builtins`, `hash_table`, `redirections` nem `signals` 🟢 |
| Observação | Os testes fazem `#include` direto de `.c` e também os listam no CMake, então pode haver símbolo duplicado no link 🟡. A lib `ft` do CMake não inclui `ft_strcmp.c` nem os `ft_lst*` 🟢 |
| Integração com o Makefile | Nenhuma. O `fclean` remove `tests_minishell`, mas nenhum alvo gera esse arquivo 🟢 |

## 9. Banco de dados

Não se aplica. Nenhum DDL, migration ou ORM 🟢.

## 10. Pontos de atenção para as próximas fases

- O nome do arquivo `sintax_analysis.c` tem erro de grafia (*syntax*). Isso é um fato, não um defeito funcional.
- O union `bin` / `fd[2]` em `t_pipe_list` mistura dados de comando e de pipe no mesmo nó. Vale investigar.
- Há uma variável global usada pelos handlers de sinal (restrição da 42: uma única global).
- `src/exec/` acumula responsabilidades bem diferentes: expansão, heredoc, path e execução.
