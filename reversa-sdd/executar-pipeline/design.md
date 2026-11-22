# Executar Pipeline: Design Técnico

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Interface

| Símbolo | Assinatura | Retorno | Observação |
|---------|-----------|---------|------------|
| `open_fds` | `(t_line *line)` | `void` | Para cada nó `PIPE`: `pipe(node->fd)`; em erro escreve `pipe error` 🟢 |
| `find_input` | `(t_pipe_list *node)` | `int` (fd de entrada, 0 ou -1) | Se o nó **anterior** é `PIPE` (ou `REDI`/`HERE`): `dup2(prev->fd[0], 0)` 🟢 |
| `find_output` | `(t_pipe_list *node)` | `void` | Procura para frente até o próximo `PIPE`; sem redirecionamento de saída, usa `pipe->fd[1]` 🟢 |
| `close_fds` | `(t_pipe_list *node)` | `void` | Volta ao início da lista e fecha as duas pontas de todo `PIPE` e `HERE` 🟢 |
| `init_fork` | `(t_line*, t_pipe_list*, t_hash_table**)` | `void` | Depois do fork, fecha no pai as duas pontas do `PIPE` anterior ao nó 🟢 |
| `wait_child` (static) | `(t_line *line)` | `void` | `waitpid(pid[i], &status_code, WUNTRACED)` até `pid[i] == 0` 🟢 |

Os descritores do pipe ficam no `union` do nó `PIPE`: `fd[0]` (leitura) e `fd[1]` (escrita) 🟢.

## Fluxo Principal
Exemplo `A | B | C` → lista `[A] [PIPE₁] [B] [PIPE₂] [C]` 🟢.

1. `open_fds` (pai): `pipe(PIPE₁.fd)`, `pipe(PIPE₂.fd)`
2. `exec_list` percorre os nós de comando; para cada um, `init_fork`:
   - **filho de A:** `find_input` (sem anterior) → stdin original; `find_output` encontra `PIPE₁` → `dup2(PIPE₁.fd[1], 1)`; `close_fds` fecha todas as pontas; `execve`
   - **pai, depois de A:** o anterior de A não é `PIPE` → nada a fechar
   - **filho de B:** anterior = `PIPE₁` → `dup2(PIPE₁.fd[0], 0)`; saída → `PIPE₂.fd[1]`; `close_fds`; `execve`
   - **pai, depois de B:** fecha `PIPE₁.fd[0]` e `PIPE₁.fd[1]`
   - **filho de C:** anterior = `PIPE₂` → `dup2(PIPE₂.fd[0], 0)`; nenhum `PIPE` à frente → stdout original
   - **pai, depois de C:** fecha `PIPE₂`
3. `wait_child` espera A, B e C nessa ordem; `status_code` termina com o status de C 🟢
4. `update_status_code` grava `$?` 🟢

### Caso builtin no último nó (`A | exit 3`)
`exec_list` testa `bin == "builtin" && next == NULL` **antes** de `init_fork`; `exec_builtins` roda no pai sem `find_input`, e por isso o pipe é ignorado. O fechamento do `PIPE` anterior, que fica em `init_fork`, **não** acontece para esse nó; os fds do pipe são fechados em `close_fds(temp)` no fim de `exec_list`, com `temp == NULL`, ou seja, **não** são fechados: depois de 2 linhas `echo a | pwd`, `ls /proc/self/fd` mostra 4 fds extras 🟢 (observado na revisão de 2026-11-22). `wait_child` espera só `A`, então `$?` = status de A (PL-03) 🟢.

## Fluxos Alternativos
- **`|` no fim da linha:** o último nó é `PIPE`; `find_output` do comando anterior escreve em `PIPE.fd[1]`; ninguém lê, e o pai fecha as duas pontas depois. Saída descartada 🟢 (observado)
- **Comando inexistente no meio:** o filho sai com 127 antes do `execve`, já com os fds fechados; o seguinte recebe EOF 🟢 (observado)
- **Nó de comando ignorado (BUG-20) no fim:** o pipe anterior fica sem leitor e o comando anterior falha ao escrever ou ler 🟢 (observado: `Bad file descriptor`)
- **Falha de `pipe()`:** só a mensagem `pipe error`; a execução segue com fds inválidos 🟡

## Dependências
- **executar-comando-simples:** lista de nós, `init_fork`, `exec_path`, `exec_list`
- **redirecionar-entrada-e-saida:** compartilha `find_input`/`find_output`; o redirecionamento explícito vence o pipe
- **POSIX:** `pipe`, `dup2`, `close`, `fork`, `waitpid`

## Decisões de Design Identificadas

| Decisão | Evidência no código | Confiança |
|---------|---------------------|-----------|
| Todos os pipes criados no pai antes dos forks | `src/facade.c:55`, `src/fd_handler_utils.c:15-28` | 🟢 |
| Cada filho fecha **todos** os fds de pipe da lista depois do `dup2` | `src/fd_handler.c:60-83` | 🟢 |
| O pai fecha cada pipe assim que o comando à direita foi criado | `src/exec/init_fork.c:25-29` | 🟢 |
| A ligação de entrada olha só o nó **imediatamente anterior** (lista dupla, ADR-005) | `src/fd_handler.c:9-12` | 🟢 |
| Builtin final no pai, mesmo em pipeline (ADR-009) | `src/exec/exec_list.c:30-32` | 🟢 |

## Estado Interno
`line->pid[]` com `count_cmds + 1` posições (a última fica 0 e marca o fim); `pid_index` avança a cada fork. Os dois são liberados e zerados em `free_line` 🟢.

## Observabilidade
Nenhuma mensagem própria, além de `pipe error` em falha de `pipe()` 🟢.

## Riscos e Lacunas
- 🟢 BUG-02: builtin final executa no shell (`ls | exit`)
- 🟢 PL-03: `$?` errado com builtin final
- 🟢 Os fds do pipe anterior a um builtin final continuam abertos no pai depois da linha (2 por linha) (observado na revisão de 2026-11-22)
- 🟡 `wait_child` para no primeiro `pid == 0`; um `fork` que falhe (`-1`) é esperado como "qualquer filho"
- ✅ L-07 (decidida em 2026-11-22): documentar o as-is. Nota para uma eventual reimplementação corretiva, fora do escopo atual: rodar todo builtin de pipeline em subshell, como o bash, muda também RE-01
