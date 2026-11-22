# Fluxograma: módulo `signals`

> Arqueólogo · 2026-11-11 · 🟢 extraído de `src/signals.c` e `src/signals_utils.c`

```mermaid
stateDiagram-v2
    [*] --> Prompt
    Prompt: Prompt (signals)
    Prompt: SIGINT → nova linha + redisplay
    Prompt: SIGQUIT → ignorado

    HeredocFork: Fork do heredoc (signal_ignore)
    HeredocFork: pai e filho ignoram SIGINT/SIGQUIT

    HeredocChild: Filho do heredoc (signal_here)
    HeredocChild: SIGINT → libera tudo, exit 130

    Exec: Pai executando (signals_parent)
    Exec: SIGINT → status 130, nova linha
    Exec: SIGQUIT → 'Quit' em stderr

    Prompt --> HeredocFork: linha válida
    HeredocFork --> HeredocChild: fork (filho)
    HeredocChild --> HeredocFork: exit
    HeredocFork --> Exec: antes de cada fork de comando
    HeredocFork --> Prompt: só builtins no pai / nenhum comando
    Exec --> Prompt: próxima iteração do loop
```

Os filhos que fazem `execve` voltam ao comportamento padrão dos sinais. Os filhos que executam builtins herdam `sig_parent`/`sig_write`.
