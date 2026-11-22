# Fluxograma: módulo `exec`

> Arqueólogo · 2026-11-11 · 🟢 extraído de `src/exec/*.c`

## Expansão (`expand_var`)

```mermaid
flowchart TD
    A([para cada token i]) --> B{lex == VAR e tks 0 diferente de aspa simples?}
    B -- não --> N[próximo]
    B -- sim --> C{token contém '$'?}
    C -- não --> N
    C -- sim --> D[key = texto após o primeiro '$'<br/>até um delimitador]
    D --> E{search_item key}
    E -- achou --> F[prefixo + valor + sufixo] --> C
    E -- não achou --> G[token = vazio<br/>o token inteiro some] --> N
```

## Montagem da lista (`creat_list_cmd`)

```mermaid
flowchart TD
    A([i = 0]) --> B{tks i?}
    B -- não --> Z([lista pronta])
    B -- sim --> C{lex i}
    C -- WORD/VAR --> D[size = quantidade de WORD/VAR seguidos]
    C -- PIPE --> E[size = 1]
    C -- REDI/REDO/REDA/HERE --> F[size = 2]
    D --> G[args = cópia de ctks i..i+size]
    E --> H[args 0 = categoria]
    F --> H
    G --> I[new_node + add_back]
    H --> I
    I --> J[i += size] --> B
```

## Execução (`exec_list` / `init_fork` / `exec_path`)

```mermaid
flowchart TD
    A([para cada nó]) --> B{is_command?}
    B -- não --> A
    B -- sim --> C{bin == 'builtin'<br/>e next == NULL?}
    C -- sim --> D[exec_builtins NO PAI]
    C -- não --> E[signals_parent → fork]
    E -->|filho| F[close_std_fd]
    F --> G[find_input / find_output / close_fds]
    G --> H{input < 0?}
    H -- sim --> X1[exit 1]
    H -- não --> I{bin == NULL?}
    I -- sim --> X2['command not found' → exit 127]
    I -- não --> J{bin == 'builtin'?}
    J -- sim --> K[exec_builtins → exit 0 sempre]
    J -- não --> L[execve bin, args, envp ORIGINAL]
    L -- falhou --> X3[perror → exit 127]
    E -->|pai| M[guarda pid e fecha o pipe anterior] --> A
    D --> A
    A -- fim --> W[waitpid de cada pid em ordem]
    W --> U[update_status_code → '?' na hash]
```

## Heredoc

```mermaid
sequenceDiagram
    participant P as Pai (facade)
    participant C as Filho do heredoc
    P->>P: creat_here_doc: pipe() por nó HERE
    P->>C: fork
    P->>P: SIGINT/SIGQUIT = IGN
    C->>C: SIGINT = sig_here (exit 130)
    loop para cada nó HERE
        loop até uma linha == delimitador ou EOF
            C->>C: readline("> ")
        end
        C->>C: write fd1 com o buffer, depois fecha fd0 e fd1
    end
    C-->>P: exit(0)
    P->>P: waitpid (status ignorado)
    P->>P: close(fd[1]) de cada HERE
    Note over P: fd[0] vira o stdin do comando em find_input
```
