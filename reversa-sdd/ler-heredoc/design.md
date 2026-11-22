# Ler Heredoc: Design Técnico

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Interface

| Símbolo | Assinatura | Retorno | Observação |
|---------|-----------|---------|------------|
| `creat_here_doc` | `(t_pipe_list *list)` | `void` | `pipe(node->fd)` em cada nó `HERE`; em erro `perror("pipe()")` 🟢 |
| `here_doc_verification` | `(t_line *line)` | `void` | No filho: para cada `HERE`, lê e escreve 🟢 |
| `here_doc_buffer` | `(t_pipe_list *node)` | `char *` ou `NULL` | Conteúdo com `\n` no fim de cada linha 🟢 |
| `generation_content` (static) | `(t_pipe_list *node, struct s_hd *hd)` | `void` | Laço `readline("> ")` 🟢 |
| `putting_nl` / `put_nl_in_buf` (static) | `(struct s_hd *)` / `(char *temp, char *content)` | `void` / `char *` | Acrescenta a linha e `\n` 🟢 |
| `here_doc_write` | `(char *buffer, t_pipe_list *node)` | `void` | `write(fd[1])` e fecha as duas pontas (no filho) 🟢 |
| `close_here_doc` | `(t_pipe_list *list)` | `void` | No pai: fecha `fd[1]` de cada `HERE` 🟢 |
| `signal_here` / `sig_here` | `(t_line *)` / `(int)` | `void` | SIGINT no filho → `\n`, libera tudo e `exit(130)` 🟢 |

Nó `HERE`: `args = {"HERE", delimitador_sem_aspas}`; `fd[0]` (leitura, usada pelo comando) e `fd[1]` (escrita, usada pelo filho) 🟢.

## Fluxo Principal
Dentro de `exec_pipe_line` (`src/facade.c:38-52`) 🟢:
1. `creat_list_cmd` já montou a lista; o delimitador veio de `ctks` (sem aspas)
2. `creat_here_doc(list)`: um pipe por `HERE`
3. `pid = fork()` (sempre, mesmo sem heredoc na linha)
4. `signal_ignore(line)` no pai **e** no filho: SIGINT e SIGQUIT ignorados
5. **Filho:**
   1. `signal_here`: SIGINT → `sig_here`
   2. `here_doc_verification`: para cada nó `HERE`, na ordem:
      ```
      buffer = NULL
      repita:
        linha = readline("> ")
        se NULL: "<delim>: Wanted" em stderr; pare
        se strlen(linha) == strlen(delim) e linha == delim: pare
        buffer += linha + "\n"
      write(fd[1], buffer); close(fd[0]); close(fd[1])
      ```
   3. `close_std_fd`, `free_line`, `free_table`, `exit(0)`
6. **Pai:** `waitpid(pid, &status_code, 0)` (o status não é examinado), `close_here_doc` fecha `fd[1]` de cada `HERE`
7. A execução segue (`list_generation_bin`, `open_fds`, `exec_list`); o comando recebe `fd[0]` como stdin por `find_input` (`is_redirect_input` inclui `HERE`) 🟢

## Fluxos Alternativos
- **EOF:** sai do laço daquele heredoc com o conteúdo parcial e passa para o próximo 🟢 (observado)
- **Ctrl-C:** `sig_here` faz `printf("\n")`, fecha `std_fd`, libera a linha e a tabela e sai com 130. As pontas de escrita restantes fecham com o processo; o pai não olha o status e executa o comando com stdin vazio (BUG-15) 🟢 (observado)
- **Linha sem heredoc:** o fork acontece do mesmo jeito; o filho não faz nada e sai com 0 🟢
- **Conteúdo grande:** `write` bloqueia quando o pipe enche, porque ninguém lê até o filho terminar; o pai espera o filho → trava (RE-09) 🟢 (observado na revisão de 2026-11-22)

## Dependências
- **GNU Readline:** `readline("> ")`
- **executar-comando-simples:** nós `HERE` na lista (`get_arg_size` = 2)
- **redirecionar-entrada-e-saida:** `find_input` liga `fd[0]`; `close_fds` fecha as pontas no filho do comando
- **tratar-sinais:** `signal_ignore`, `signal_here`, `sig_here`

## Decisões de Design Identificadas

| Decisão | Evidência no código | Confiança |
|---------|---------------------|-----------|
| Heredoc lido em processo filho separado, antes dos comandos (ADR-006) | `src/facade.c:40-50` | 🟢 |
| Conteúdo transportado por `pipe()`, não por arquivo temporário | `src/exec/here_doc_utils.c:42-54` | 🟢 |
| O fork do heredoc é incondicional | `src/facade.c:40` | 🟢 |
| O status do filho do heredoc é ignorado | `src/facade.c:51` | 🟢 |
| Comparação do delimitador por tamanho + `ft_strncmp`, para evitar o problema de prefixo | `src/exec/here_doc.c:46-48` | 🟢 |

## Estado Interno
`struct s_hd { content, temp, buffer }` local à leitura. `temp` guarda o conteúdo depois da 1ª linha; `buffer`, a partir da 2ª. `here_doc_buffer` devolve `buffer` ou, se só houve uma linha, `temp` 🟢.

## Observabilidade
Prompt `> `, mensagem `<delim>: Wanted` em stderr e `\n` no Ctrl-C 🟢.

## Riscos e Lacunas
- 🟢 BUG-15: Ctrl-C não cancela a linha; `$?` não vira 130
- 🟢 RN-27: sem expansão
- 🟢 RE-09: conteúdo maior que o pipe trava o shell (observado na revisão de 2026-11-22)
- 🟡 O fork incondicional custa um processo por linha executada
- 🟢 `status_code` é sobrescrito pelo `waitpid` do heredoc antes da execução. É isso que zera `$?` para um builtin final sem filhos (`ls /nope` seguido de `pwd` → `$?=0`) 🟢 (observado). Remover o fork incondicional exige zerar `status_code` explicitamente
- ✅ L-07 (decidida em 2026-11-22): documentar o as-is. Nota para uma eventual reimplementação corretiva, fora do escopo atual: arquivo temporário (como o bash) resolve RE-09; tratar o status do filho resolve BUG-15
