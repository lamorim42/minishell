# Executar Comando Simples: Design Técnico

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Interface

| Símbolo | Assinatura | Retorno | Observação |
|---------|-----------|---------|------------|
| `creat_list_cmd` | `(t_line *line)` | `void` | Preenche `line->list_cmds` 🟢 |
| `make_args` | `(t_line *line, int *index)` | `char **` | Copia `size` tokens de `ctks`; em operador, troca `args[0]` pelo lexema 🟢 |
| `get_arg_size` | `(t_line *line, int index)` | `int` | `WORD`/`VAR` consecutivos → n; `PIPE` → 1; redirecionamento → 2 🟢 |
| `new_node` / `add_back_list` | `(char **args)` / `(t_pipe_list **, t_pipe_list *)` | nó / `void` | Lista duplamente encadeada 🟢 |
| `list_generation_bin` | `(t_line *line, t_hash_table **table)` | `void` | Preenche `bin` de cada nó de comando 🟢 |
| `path_finder` | `(char *cmd, t_hash_table **table)` | `char *` ou `NULL` | Primeiro `dir/cmd` com `access(F_OK) == 0` 🟢 |
| `is_a_builtin` | `(char **node)` | `FALSE` se builtin, `TRUE` se não (lógica invertida) | Comparação por prefixo 🟢 |
| `is_command` | `(t_pipe_list *node)` | bool | `args[0]` diferente (por prefixo) dos 5 lexemas de operador 🟢 |
| `exec_list` | `(t_line *line, t_hash_table **table)` | `void` | Despacha, espera e atualiza `$?` 🟢 |
| `init_fork` | `(t_line *line, t_pipe_list *list, t_hash_table **table)` | `void` | Fork do comando; fecha pipes no pai 🟢 |
| `exec_path` | `(t_line *line, t_pipe_list *list, t_hash_table **table)` | não retorna | Roda no filho 🟢 |
| `exec_builtins` | `(t_pipe_list *node, t_hash_table **table)` | `void` | Igualdade exata com os 7 nomes 🟢 |

### Estrutura `t_pipe_list` (`includes/minishell.h`)
| Campo | Tipo | Uso |
|-------|------|-----|
| `args` | `char **` | comando: `argv`; operador: `{lexema, alvo}` ou `{"PIPE"}` |
| `bin` / `fd[2]` | `union` | comando: caminho ou `"builtin"`; operador: descritores |
| `next`, `prev` | `t_pipe_list *` | encadeamento duplo (ADR-005) |

## Fluxo Principal
Dentro de `exec_pipe_line` (`src/facade.c:32-59`) 🟢:
1. `creat_list_cmd`: percorre `tks`; em cada posição, `make_args` copia `get_arg_size` tokens de `ctks` e avança o índice
2. `creat_here_doc` e o filho do heredoc (unit `ler-heredoc`)
3. `list_generation_bin` (`src/exec/list_generation_bin.c:17-36`):
   ```
   para cada nó:
     se is_a_builtin(args[0]) == 0:    bin = "builtin"
     senão se is_command(nó):
        se args[0] começa com "/", "./" ou "../": bin = strdup(args[0])
        senão: bin = path_finder(args[0])          # NULL se não achar
   ```
4. `alloc_commands`: conta os nós de comando (`count_cmds`) e aloca `pid[count_cmds + 1]` zerado
5. `open_fds` (unit `redirecionar-entrada-e-saida`)
6. `exec_list` (`src/exec/exec_list.c:18-40`):
   ```
   para cada nó:
     se !is_command(nó): continua
     se bin == "builtin" e nó->next == NULL: exec_builtins(nó)    # no pai
     senão: init_fork(nó)
   wait_child: waitpid(pid[i], &status_code, WUNTRACED) para cada pid
   update_status_code
   ```
7. `init_fork` (`src/exec/init_fork.c:17-42`): `signals_parent`, `fork`; no filho `close_std_fd` e `exec_path`; no pai guarda o pid e fecha o pipe do nó anterior
8. `exec_path` no filho (`src/exec/exec_path.c:17-39`):
   ```
   input = make_redirections(nó)       # find_input, find_output, close_fds
   se input < 0:            free_exit(1)
   se bin == NULL:          "<nome>: command not found" em stderr; free_exit(127)
   se bin == "builtin":     exec_builtins; free_exit(0)
   execve(bin, args, line->envp)       # envp original de main
   em erro: perror(args[0]); free_exit(127)
   ```
9. `add_history(line->str)` e `free_line` (`src/facade.c:57-58`)

### Algoritmo `path_finder`
```
path = search_item(table, "PATH")          # cópia; NULL se não existir
se NULL: retorna NULL
para cada dir em split(path, ':'):
   cand = dir + "/" + cmd
   se access(cand, F_OK) == 0: retorna cand
retorna NULL
```

### Semântica de `ft_strncmp` (causa de BUG-04 e BUG-20)
`ft_strncmp(a, b, n)` para no primeiro `\0` de **qualquer** das strings e retorna 0 se não houve diferença até ali (`Libft/ft_strncmp.c`) 🟢. Logo:
- `ft_strncmp("", X, n) == 0` para todo `X`
- `ft_strncmp("ec", "echo", 4) == 0` e `ft_strncmp("pwdx", "pwd", 3) == 0`

Efeitos: `is_a_builtin` aceita prefixos nos dois sentidos; `is_command` rejeita `""`, `P`, `PI`, `R`, `RE`, `RED`, `H`, `HE`, `HER` etc. Um nó de comando rejeitado por `is_command` não recebe `bin`, não é contado em `count_cmds` e é pulado por `exec_list` 🟢 (observado no gdb: `$NOPE ls` → nó `{"", "ls"}`, `bin = NULL`, `count_cmds = 0`).

## Fluxos Alternativos
- **Builtin final:** `exec_builtins` no pai; nenhum pid é registrado; `wait_child` não espera nada e `update_status_code` usa o `status_code` corrente 🟢
- **Builtin não final** (seguido de redirecionamento ou pipe): roda no filho e sai com 0 (BUG-10) 🟢
- **Nome igual a prefixo de builtin, mas não exato** (`pwdx`): `bin = "builtin"` → `exec_builtins` cai no `else`: grava `status_code = 127` na global e escreve `command not found` 🟢
- **`PATH` ausente:** `path_finder` → `NULL` → `command not found` no filho 🟢
- **Falha de `fork`:** não tratada; `pid = -1` é guardado e esperado 🟡

## Dependências
- **analisar-linha-de-comando / expandir-variaveis:** `lex` e `ctks`
- **hash_table:** `search_item("PATH")`; `update_status_code` grava `?`
- **redirecionar-entrada-e-saida:** `open_fds`, `find_input`, `find_output`, `close_fds`
- **builtins:** `exec_builtins` despacha para as units `navegar-diretorios`, `imprimir-texto-echo`, `gerenciar-variaveis-de-ambiente`, `encerrar-shell`
- **tratar-sinais:** `signals_parent` antes de cada fork
- **POSIX:** `fork`, `execve`, `waitpid`, `access`

## Decisões de Design Identificadas

| Decisão | Evidência no código | Confiança |
|---------|---------------------|-----------|
| Lista duplamente encadeada de nós de comando e de operador (ADR-005) | `src/exec/make_list.c` | 🟢 |
| `union` `bin`/`fd` no nó: o tipo do nó define qual campo vale | `includes/minishell.h` | 🟢 |
| Resolução de `PATH` pela tabela, não por `getenv` (ADR-008) | `src/exec/path_finder.c:22` | 🟢 |
| Builtin no pai quando é o último nó (ADR-009) | `src/exec/exec_list.c:30-32` | 🟢 |
| `execve` com o `envp` de `main` | `src/exec/exec_path.c:34` | 🟢 |
| Categorias e nomes comparados por prefixo com `ft_strncmp` | `src/exec/exec_utils.c:15-22`, `src/builtins/builtins_utils.c:40-51` | 🟢 |

## Estado Interno
- `line->list_cmds`: uma lista por linha, liberada em `free_line` 🟢
- `line->pid[]`, `pid_index`, `count_cmds`: por linha; `free_line` libera `pid` e zera `count_cmds` (`src/free.c:39-44`) 🟢
- `line->status_code`: último status bruto de `waitpid`; persiste entre linhas 🟢

## Observabilidade
Mensagens em stderr: `<nome>: command not found` (`error_msg`) e `perror(<nome>)` 🟢. Nenhuma mensagem para os comandos ignorados por BUG-20 🟢.

## Riscos e Lacunas
- 🟢 BUG-20 (novo): comandos com nome vazio ou prefixo de lexema somem em silêncio
- 🟢 BUG-04: programas externos com prefixo de builtin não executam
- 🟢 BUG-03: `envp` original no `execve`
- 🟢 ML-04: argumentos depois de redirecionamento viram comando
- 🟡 `fork` sem tratamento de erro; `pid` negativo seria esperado com `waitpid(-1)`
- 🟡 `free_list` também usa `ft_strncmp` por prefixo para decidir o que liberar (`src/free.c:58-80`); nós com nome vazio podem não ser liberados
- ✅ L-07 (decidida em 2026-11-22): documentar o as-is. Nota para uma eventual reimplementação corretiva, fora do escopo atual: corrigir `ft_strncmp` (ou trocar por igualdade exata) muda o comportamento de várias units ao mesmo tempo; a decisão precisa ser única
