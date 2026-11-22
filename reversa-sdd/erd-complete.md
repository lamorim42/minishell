# ERD: modelo de dados em memória

> Gerado pelo **Arquiteto** (Reversa) em 2026-11-11
> O minishell não tem banco de dados. Este ERD representa as **estruturas em memória** e suas relações. Os campos estão detalhados em `data-dictionary.md`.

```mermaid
erDiagram
    S_MINISHELL ||--|| T_LINE : "line (ponteiro)"
    S_MINISHELL ||--|| T_HASH_TABLE : "table (ponteiro)"
    T_LINE ||--o{ T_PIPE_LIST : "list_cmds (cabeça)"
    T_PIPE_LIST |o--o| T_PIPE_LIST : "next / prev"
    T_HASH_TABLE ||--o{ T_HASH_ITEM : "item[149] (buckets)"
    T_HASH_ITEM |o--o| T_HASH_ITEM : "next (colisão)"
    T_LINE ||--o{ TOKEN : "tks / lex / ctks (paralelos)"

    S_MINISHELL {
        t_line_ptr line
        t_hash_table_ptr table
    }
    T_LINE {
        string str "linha crua"
        int_arr std_fd "dup de stdin/stdout"
        string_arr tks "tokens crus"
        string_arr lex "categorias"
        string_arr ctks "tokens limpos"
        int tks_nbr
        string_arr envp "envp original"
        int status_code
        int count_cmds
        int_arr pid
        int pid_index
        int sig "não lido"
        int_arr fd "não usado"
    }
    T_PIPE_LIST {
        string_arr args "args[0] discrimina o papel"
        string bin "UNION: só em comandos"
        int_arr fd "UNION: só em operadores"
    }
    T_HASH_TABLE {
        int size "149"
        int count "não lido"
    }
    T_HASH_ITEM {
        string key PK "único por convenção (não garantido)"
        string value
    }
    TOKEN {
        int index PK
        string tks
        string lex "WORD|VAR|PIPE|REDI|REDO|REDA|HERE"
        string ctks
    }
```

## Cardinalidades e invariantes

| Relação | Cardinalidade | Invariante esperada | Garantida? |
|---------|---------------|---------------------|------------|
| `s_minishell` → `t_line` | 1:1 | Instância única | 🟢 sim |
| `t_line` → `t_pipe_list` | 1:0..N | Lista vazia em linha só com espaços | 🟢 |
| `t_pipe_list` ↔ `t_pipe_list` | encadeamento duplo | `n.next.prev == n` | 🟢 (`add_back_list`) |
| `t_line` → tokens | 1:N | `len(tks) == len(lex) == len(ctks) == tks_nbr` | 🟡 depende de `count_tks` e `tokenizer` concordarem |
| `t_hash_table` → `t_hash_item` | 1:0..N | **Chave única** | ✗ inserção só deduplica contra a cabeça do bucket (🟢) |
| `T_HASH_ITEM.key` | — | Sempre existe `"?"` | 🟢 |

*TOKEN não é uma struct real: representa as posições `i` dos três arrays paralelos (ADR-003).*
