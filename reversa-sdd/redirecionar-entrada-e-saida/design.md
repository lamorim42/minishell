# Redirecionar Entrada e Saída: Design Técnico

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Interface

| Símbolo | Assinatura | Retorno | Observação |
|---------|-----------|---------|------------|
| `open_fds` | `(t_line *line)` | `void` | No pai, depois do heredoc e antes de `exec_list` (`src/facade.c:55`) 🟢 |
| `check_valid_fd` (static) | `(t_line *line, t_pipe_list *list, char *flag)` | `void` | `"TRUNC"` ou `"APPEND"`; em erro `perror` + `free_line(line)` 🟢 |
| `find_input` | `(t_pipe_list *node)` | `int`: último fd de entrada, 0 ou -1 | Em erro escreve `<alvo>: No such file or directory` 🟢 |
| `find_output` | `(t_pipe_list *node)` | `void` | `dup2` e `close` do fd escolhido 🟢 |
| `close_fds` | `(t_pipe_list *node)` | `void` | Volta ao início e fecha os fds de todos os operadores 🟢 |
| `is_redirect_output` / `is_redirect_input` | `(t_pipe_list *node)` | bool | `REDO`/`REDA` e `REDI`/`HERE` 🟢 |
| `file_exists` | `(int fd)` | bool | `fstat(fd) == 0` (`src/main.c:76-81`) 🟢 |

Nó de redirecionamento: `args = {"REDO"|"REDA"|"REDI"|"HERE", alvo}` e `fd[0]` = descritor aberto (no `HERE`, `fd` = pipe do conteúdo) 🟢.

## Fluxo Principal
1. **Abertura** (`open_fds`, pai, `src/fd_handler_utils.c:15-44`):
   ```
   para cada nó:
     REDI → fd[0] = open(alvo, O_RDONLY)              # -1 se não existir, sem mensagem aqui
     REDO → fd[0] = open(alvo, O_RDWR|O_CREAT|O_TRUNC, 0644)
     REDA → fd[0] = open(alvo, O_RDWR|O_CREAT|O_APPEND, 0644)
        se -1: perror(alvo); free_line(line)          # o laço continua sobre nós liberados (BUG-01)
     PIPE → pipe(fd)
   ```
2. **No pai, por nó** (`init_fork`, `src/exec/init_fork.c:22-24`): ao passar por um nó `REDO`/`REDA`, fecha `fd[0]` 🟢. Os `REDI` não são fechados no pai 🟢 (observado na revisão de 2026-11-22)
3. **No filho do comando** (`make_redirections`, `src/exec/exec_path.c:41-49`):
   1. `find_input` (`src/fd_handler.c:15-42`):
      ```
      se prev é PIPE, REDI ou HERE: dup2(prev->fd[0], 0)
      input = 0
      para cada nó seguinte até PIPE:
         se REDI/HERE: input = nó->fd[0]
         se !file_exists(nó->fd[0]): "<nó->args[1]>: No such file or directory"; retorna -1
      se input > 0: dup2(input, 0)
      retorna input
      ```
   2. `find_output` (`src/fd_handler.c:44-70`):
      ```
      output = 1
      para cada nó seguinte:
         se PIPE: se output == 1: output = pipe->fd[1]; pare
         se REDO/REDA: output = nó->fd[0]
      se output != 1: dup2(output, 1); close(output)
      ```
   3. `close_fds`: fecha todos os fds de operadores da lista
   4. `exec_path`: se `input < 0` → `free_exit(1)`
4. **Fim da linha:** `main_loop` restaura stdin/stdout com `dup2(std_fd)` (unit `iniciar-sessao-e-ler-comando`) 🟢

### Por que `> f4 echo d` ignora o arquivo
A lista é `[REDO f4] [echo d]`. `find_output` só olha para **frente** a partir do comando, e o `REDO` está atrás. `find_input` olha para trás, mas só procura entrada 🟢.

### Por que `ls | > o1 wc -l` não lê o pipe
A lista é `[ls] [PIPE] [REDO o1] [wc -l]`. Para `wc`, o `prev` é `REDO`, que não satisfaz `prev é PIPE || is_redirect_input(prev)`; nenhum `dup2` de entrada é feito e o laço para frente não encontra `REDI`/`HERE`. O stdin continua o original (o terminal) 🟢. `find_output` olha só para frente e não acha `REDO`, então a saída também é a original 🟢. O `REDO o1` fica atrás de `wc` e só tem o efeito de criar/truncar `o1` em `open_fds` 🟢. [Revisão 2026-11-22: corrigida a versão anterior, que dizia que `wc` lia `o1`.]

### Verificação `file_exists` em nós de comando (RR-07)
Dentro do laço de `find_input`, `file_exists(temp->fd[0])` também é chamado em **nós de comando** seguintes (`cat < f -e` → nó `-e`). Nesses nós, `fd[0]` é o pedaço baixo do ponteiro `bin` (union), e o resultado depende do valor do ponteiro 🟡.

## Fluxos Alternativos
- **Entrada inexistente depois do comando:** `find_input` acha `fd[0] = -1` → mensagem → `free_exit(1)` 🟢 (observado)
- **Entrada inexistente antes do comando:** `prev` é `REDI` com `fd[0] = -1`; `dup2(-1, 0)` falha em silêncio e o laço para frente não encontra nada; o comando roda 🟢 (observado)
- **Saída impossível:** `free_line` dentro de `open_fds` libera a lista que o laço ainda percorre; o retorno a `exec_pipe_line` usa a linha liberada → *segfault* ou *abort* 🟢 (observado: código 139)

## Dependências
- **executar-comando-simples:** lista de nós; `exec_path` chama `make_redirections`
- **executar-pipeline:** compartilha `find_input`/`find_output` (ramo `PIPE`)
- **ler-heredoc:** fornece o `fd` dos nós `HERE`
- **POSIX:** `open`, `dup2`, `close`, `fstat`

## Decisões de Design Identificadas

| Decisão | Evidência no código | Confiança |
|---------|---------------------|-----------|
| Todos os arquivos abertos no pai antes da execução | `src/facade.c:55` | 🟢 |
| `dup2` feito no filho, perto do `execve` | `src/exec/exec_path.c:21` | 🟢 |
| Entrada procurada atrás (1 nó) e à frente; saída só à frente | `src/fd_handler.c:15-70` | 🟢 |
| Arquivos de saída abertos com `O_RDWR`, não `O_WRONLY` | `src/fd_handler_utils.c:33-37` | 🟢 |
| Validação dos fds com `fstat` em vez do retorno de `open` | `src/fd_handler.c:20`, `src/main.c:76-81` | 🟢 |

## Estado Interno
Os fds vivem no `union` de cada nó durante a linha. O pai fecha os de saída em `init_fork`; os de pipe e heredoc em `init_fork`/`close_here_doc`. Os de entrada (`REDI`) ficam abertos e vazam para os filhos seguintes 🟢 (observado na revisão de 2026-11-22).

## Observabilidade
`perror(alvo)` (saída), `<alvo>: No such file or directory` (entrada, escrito pelo filho) 🟢.

## Riscos e Lacunas
- 🟢 BUG-01: crash quando um arquivo de saída não pode ser aberto
- 🟢 BUG-08 e BUG-09: redirecionamentos antes do comando
- 🟢 RR-04: redirecionamento entre `|` e o comando
- 🟡 RR-07: `file_exists` lido do `union` em nós de comando
- 🟢 RR-08: vazamento de fd de `REDI` por linha (observado na revisão de 2026-11-22)
- ✅ L-07 (decidida em 2026-11-22): documentar o as-is. Nota para uma eventual reimplementação corretiva, fora do escopo atual: um modelo "cada comando tem sua lista de redirecionamentos" (como no bash) resolve BUG-08, BUG-09, RR-04 e ML-04 de uma vez, mas muda a estrutura da lista
