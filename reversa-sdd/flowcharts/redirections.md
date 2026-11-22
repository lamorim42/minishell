# Fluxograma: módulo `redirections`

> Arqueólogo · 2026-11-11 · 🟢 extraído de `src/fd_handler.c` e `src/fd_handler_utils.c`

## Abertura no pai (`open_fds`)

```mermaid
flowchart TD
    A([para cada nó]) --> B{args 0}
    B -- REDI --> C[fd0 = open alvo, O_RDONLY<br/>erro: fd0 = -1 sem mensagem]
    B -- REDO --> D[fd0 = open O_RDWR / CREAT / TRUNC 0644]
    B -- REDA --> E[fd0 = open O_RDWR / CREAT / APPEND 0644]
    B -- PIPE --> F[pipe fd]
    D --> G{fd0 == -1?}
    E --> G
    G -- sim --> H[perror + free_line<br/>⚠️ o loop continua em memória liberada → CRASH]
    G -- não --> A
    C --> A
    F --> A
```

## Ligação no filho (`find_input` / `find_output`)

```mermaid
flowchart TD
    S([nó de comando N]) --> P{N.prev é PIPE, REDI ou HERE?}
    P -- sim --> P1[dup2 prev.fd0 → stdin]
    P -- não --> Q
    P1 --> Q[percorre N.next até PIPE]
    Q --> Q1{nó é REDI/HERE?}
    Q1 -- sim --> Q2[input = fd0]
    Q1 --> Q3{fstat fd0 válido?}
    Q3 -- não --> ERR['alvo: No such file or directory' → -1]
    Q3 -- sim --> Q
    Q -- fim --> R{input > 0?}
    R -- sim --> R1[dup2 input → stdin]
    R --> O[percorre N.next até PIPE]
    O --> O1{REDO/REDA?}
    O1 -- sim --> O2[output = fd0, o último vence]
    O --> O3{encontrou PIPE e output == 1?}
    O3 -- sim --> O4[output = pipe.fd1]
    O -- fim --> T{output != 1?}
    T -- sim --> T1[dup2 output → stdout e close]
```

Observação: só o nó **imediatamente anterior** ao comando é olhado para entrada, e **nenhum** nó anterior é olhado para saída. Por isso `> f cmd` não redireciona a saída.
