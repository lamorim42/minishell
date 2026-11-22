# Iniciar Sessão e Ler Comando: Design Técnico

## Interface

| Símbolo | Assinatura | Retorno | Observação |
|---------|-----------|---------|------------|
| `main` | `(int argc, char **argv, char **envp)` | `int` (nunca retorna normalmente) | Preenche a global `g_minishell` 🟢 |
| `main_loop` (static) | `(t_line *line, t_hash_table **table)` | `void` | Laço infinito 🟢 |
| `control_d` (static) | `(char *str, t_hash_table **table)` | `void` | Chama `exit(0)` se `str == NULL` 🟢 |
| `is_a_comment` (static) | `(char *str)` | `int` (bool) | Pula só `' '` 🟢 |
| `verify_quots` | `(char *str)` | `int` (bool) | Pareamento de `"` e `'` 🟢 |
| `init_line` | `(t_line *line)` | `void` | 🟢 |
| `population_hash_table` | `(t_line *line, t_hash_table **table)` | `t_hash_table *` | 🟢 |

Entradas: `envp` do processo e stdin (TTY). Saídas: prompt e mensagens em stdout/stderr, e a linha (`line->str`) entregue à Facade.

## Fluxo Principal
1. `init_line(&line)`: zera os ponteiros, `status_code = 0`, `sig = 1` (`src/init.c:15-28`) 🟢
2. `line.envp = envp`; `population_hash_table` cria a tabela com 149 buckets e insere cada `envp[i]` (`src/hash_table/create_table.c:36-57`) 🟢:
   - `ft_split(envp[i], '=')`
   - se houver mais de um `=`, `preper_to_insert` junta os pedaços do valor de volta com `=`
   - `K` sem valor → `""`
   - por fim, `hash_insert("?", "0")`
3. `g_minishell.line = &line; g_minishell.table = table` (`src/main.c:32-33`) 🟢
4. `std_fd[IN] = dup(0); std_fd[OUT] = dup(1)` (`src/main.c:40-41`) 🟢
5. Laço (`src/main.c:42-61`):
   1. `signals(line)`: SIGINT → `sig_handler`, SIGQUIT → ignorado
   2. `line->str = readline("miau> ")`
   3. `control_d`: se `NULL`, faz `free_table`, `write(2, "exit\n", 6)`, `close_std_fd`, `exit(0)`
   4. se `!verify_quots(str)`: libera, `printf("Syntax ERROR!\n")`, `continue`
   5. se `is_a_comment(str)`: libera, `continue`
   6. `facade(line, table)` (ver as units 2 a 7)
   7. `dup2(std_fd[IN], 0); dup2(std_fd[OUT], 1)`

### Algoritmo `verify_quots`
```
i = 0; ok = TRUE
enquanto str[i]:
  se str[i] é '"' ou '\'':
     q = str[i]; i++
     avança até str[i] == q ou fim
     se achou q: i++, ok = TRUE  senão ok = FALSE
  senão i++
  se !ok: pare
retorna ok
```
Aspas de um tipo dentro de aspas do outro tipo são literais 🟢.

## Fluxos Alternativos
- **Linha vazia:** `facade` verifica `ft_strlen > 0`; senão só libera 🟢
- **Ctrl-C no prompt:** tratado na unit `tratar-sinais` (novo prompt, `$?` não muda) 🟢
- **Comentário com aspas desbalanceadas:** erro de sintaxe, porque `verify_quots` vem primeiro 🟢

## Dependências
- **GNU Readline:** `readline`, `add_history` (este último na Facade)
- **hash_table:** `create_table`, `hash_insert`, `free_table`
- **signals:** `signals()`
- **Facade** (`src/facade.c`), que encadeia as units 2 a 7

## Decisões de Design Identificadas

| Decisão | Evidência no código | Confiança |
|---------|---------------------|-----------|
| A validação de aspas acontece no REPL, antes da tokenização | `src/main.c:47` | 🟢 |
| Global única com ponteiros para a linha e a tabela (ADR-007) | `src/main.c:15,32-33` | 🟢 |
| `t_line` reaproveitada entre iterações | `src/free.c:31-45` | 🟢 |
| A guarda `argv[0] == NULL && argc > 1` serve só para "usar" os parâmetros (Norminette/`-Werror`) | `src/main.c:27` | 🟡 |

## Estado Interno
- `t_line` (pilha de `main`): `str` por iteração, `std_fd` por sessão.
- `t_hash_table` (heap): durante a sessão inteira.
- `g_minishell`: aponta para os dois acima.

## Observabilidade
Não há logs. As mensagens ao usuário são `Syntax ERROR!` (stdout) e `exit` (stderr) 🟢.

## Riscos e Lacunas
- 🟡 `init_line` faz `ft_bzero(line->fd, 2)`, que zera só 2 bytes. O campo não é usado
- 🟡 `std_fd` não é verificado (`dup` pode falhar)
- ✅ L-01 (respondida em 2026-11-22): o alvo é o **mínimo exigido pelo subject** da 42; o modo não interativo (`-c`, script) fica **fora do escopo** 🟢
