# Máquinas de Estado — minishell

> Gerado pelo **Detetive** (Reversa) em 2026-11-11
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA

O minishell não tem entidades persistentes com campo de status. As "máquinas de estado" relevantes são o **ciclo de vida da sessão**, o **ciclo de vida de uma linha**, o **contexto de sinais** e o **valor de `$?`**.

---

## 1. Sessão do shell (processo principal)

```mermaid
stateDiagram-v2
    [*] --> Inicializando: ./minishell
    Inicializando --> AguardandoEntrada: init_line + hash do envp + '?'=0
    AguardandoEntrada --> AguardandoEntrada: Ctrl-C (nova linha)
    AguardandoEntrada --> AguardandoEntrada: aspas desbalanceadas / comentário / linha vazia
    AguardandoEntrada --> ProcessandoLinha: readline retorna texto
    ProcessandoLinha --> AguardandoEntrada: fim da linha (stdin/stdout restaurados)
    AguardandoEntrada --> Encerrado: Ctrl-D (exit 0)
    ProcessandoLinha --> Encerrado: builtin exit no pai
    ProcessandoLinha --> Abortado: erro ao abrir arquivo de saída (double free)
    Encerrado --> [*]
    Abortado --> [*]
```

| Transição | Gatilho | Local | Confiança |
|-----------|---------|-------|-----------|
| Inicializando → AguardandoEntrada | `main` concluído | `main.c:21-34` | 🟢 |
| AguardandoEntrada → Encerrado | `readline` retorna `NULL` | `main.c:65-74` | 🟢 |
| ProcessandoLinha → Encerrado | `exit` como último nó (inclusive `ls \| exit`) | `exit.c:58-60`, `exec_list.c:30` | 🟢 |
| ProcessandoLinha → Abortado | `open` de `>`/`>>` falha | `fd_handler_utils.c:51-55` | 🟢 (observado) |

---

## 2. Ciclo de vida de uma linha (`t_line`)

```mermaid
stateDiagram-v2
    [*] --> Lida
    Lida --> Rejeitada: aspas desbalanceadas
    Lida --> Ignorada: comentário ou vazia
    Lida --> Tokenizada: building_tokens
    Tokenizada --> Rejeitada: sintax_analysis = FALSE
    Tokenizada --> Expandida: expand_var
    Expandida --> Limpa: clean_tokens
    Limpa --> Estruturada: creat_list_cmd
    Estruturada --> HeredocsLidos: fork + here_doc_verification
    HeredocsLidos --> Resolvida: list_generation_bin
    Resolvida --> FdsAbertos: open_fds
    FdsAbertos --> Executada: exec_list + wait
    Executada --> Registrada: add_history
    Registrada --> Liberada: free_line
    Rejeitada --> Liberada: free_sintax / free
    Ignorada --> Liberada
    Liberada --> [*]
```

Campos de `t_line` preenchidos em cada estado:

| Estado | `str` | `tks` | `lex` | `ctks` | `list_cmds` | `pid` |
|--------|:-----:|:-----:|:-----:|:------:|:-----------:|:-----:|
| Lida | ✔ | | | | | |
| Tokenizada | ✔ | ✔ | ✔ | | | |
| Expandida | ✔ | ✔* | ✔ | | | |
| Limpa | ✔ | ✔ | ✔ | ✔ | | |
| Estruturada / HeredocsLidos / Resolvida | ✔ | ✔ | ✔ | ✔ | ✔ | |
| FdsAbertos / Executada | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ |
| Liberada | ✘ | NULL | NULL | NULL | NULL | NULL |

\* `tks` é alterado no lugar 🟢.

---

## 3. Papel de um nó (`t_pipe_list`)

O papel é decidido uma vez, na criação, por `args[0]`, e não muda depois. Os campos ativos é que mudam ao longo do processamento:

```mermaid
stateDiagram-v2
    [*] --> Criado: new_node (bin = NULL)
    Criado --> Comando: args[0] não é operador
    Criado --> Operador: args[0] é PIPE, REDI, REDO, REDA ou HERE
    Comando --> Resolvido: list_generation_bin (bin = path / 'builtin' / NULL)
    Operador --> HerePipe: HERE → pipe() (creat_here_doc)
    Operador --> FdAberto: REDI/REDO/REDA → open, PIPE → pipe (open_fds)
    HerePipe --> FdAberto: conteúdo escrito pelo filho, ponta de escrita fechada
    Resolvido --> EmExecucao: fork (ou builtin no pai)
    FdAberto --> Fechado: close_fds / init_fork
    EmExecucao --> Finalizado: waitpid
```

🟢 extraído de `make_list.c`, `here_doc_utils.c`, `fd_handler_utils.c`, `exec_list.c`, `init_fork.c`.

---

## 4. Contexto de sinais

Está detalhado em `flowcharts/signals.md`. Resumo:

| Estado | SIGINT | SIGQUIT | Entrada por |
|--------|--------|---------|-------------|
| `PROMPT` | novo prompt | ignorado | `signals()` |
| `IGNORANDO` | ignorado | ignorado | `signal_ignore()` (fork do heredoc) |
| `HEREDOC_FILHO` | `exit(130)` | ignorado | `signal_here()` |
| `EXECUTANDO` | `\n`, status 130 | `Quit` | `signals_parent()` |

O campo `t_line.sig` recebe 0 em cada troca de contexto, mas **nunca é lido**. Parece ser vestígio de uma máquina de estados abandonada 🟡.

---

## 5. Valor de `$?` (chave `"?"`)

```mermaid
stateDiagram-v2
    [*] --> Zero: population_hash_table ('?'='0')
    Zero --> Atualizado: exec_list → update_status_code
    Atualizado --> Atualizado: cada linha que chega a exec_list
    note right of Atualizado
      Não é atualizado em: erro de sintaxe,
      aspas desbalanceadas, Ctrl-C no prompt,
      linha vazia ou comentário.
    end note
```

Fonte do valor (`line->status_code`), em ordem de sobrescrita dentro de uma linha:
1. `waitpid` do heredoc. É sempre executado, e normalmente dá 0 🟢.
2. Builtin no pai escreve 1 ou 127 em caso de erro 🟢.
3. `sig_parent` escreve 130 🟢.
4. `waitpid` de cada filho (o último vence) 🟢.

O valor final é convertido: `WEXITSTATUS`, ou `128 + WTERMSIG`, ou o valor bruto 🟢.
