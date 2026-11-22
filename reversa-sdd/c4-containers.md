# C4 Nível 2: Containers

> Gerado pelo **Arquiteto** (Reversa) em 2026-11-11

O minishell é **um único executável**. Em tempo de execução, porém, ele cria **processos** com papéis diferentes. Para um shell, os "containers" relevantes são esses processos e as bibliotecas ligadas ao binário.

```mermaid
flowchart TB
    subgraph bin["📦 Executável ./minishell"]
        direction TB
        main["<b>Processo principal (shell)</b><br/>REPL, parsing, hash do ambiente,<br/>builtins no pai, wait<br/><i>C</i>"]
        libft["<b>libft.a</b><br/>Biblioteca estática<br/><i>C · Libft/</i>"]
        rl["<b>libreadline</b><br/>Biblioteca dinâmica<br/><i>GNU Readline</i>"]
    end
    hd["<b>Processo filho: heredoc</b><br/>Lê os heredocs com readline e<br/>escreve no pipe<br/><i>fork do principal, um por linha</i>"]
    child["<b>Processos filhos: comandos</b><br/>Um por comando do pipeline.<br/>Faz dup2 de fds e então execve ou builtin<br/><i>fork do principal</i>"]
    ext[["Programa externo<br/><i>imagem substituída via execve</i>"]]

    main -- "usa" --> libft
    main -- "readline(), add_history()" --> rl
    main -- "fork() + pipe() por nó HERE" --> hd
    hd -- "conteúdo via pipe (fd[1] para fd[0])" --> child
    main -- "fork() por comando; pipe() por PIPE; open() por redirecionamento" --> child
    child -- "execve(bin, args, envp ORIGINAL)" --> ext
    child -. "exit status" .-> main
    hd -. "exit 0 / 130 (ignorado)" .-> main
```

| Container | Tecnologia | Responsabilidade | Ciclo de vida |
|-----------|------------|------------------|---------------|
| Processo principal | C + readline | Todo o pipeline até `open_fds`, builtins no último nó, `waitpid`, `$?` | Sessão inteira 🟢 |
| Filho do heredoc | C (fork) | Ler heredocs com isolamento de SIGINT | **Toda linha válida**, mesmo sem `<<` 🟢 |
| Filhos de comando | C (fork) → `execve` | Ligar stdin e stdout (pipes, arquivos) e executar | Um por comando que não roda no pai 🟢 |
| libft.a | C estático | Strings e memória | Compilada junto 🟢 |
| libreadline | Biblioteca do sistema | Edição de linha e histórico | Dinâmica 🟢 |
| Suíte de testes `runTests` | C++ / GoogleTest / CMake | Testes unitários de `tokens/` | Build separado, fora do Makefile 🟢 |

## Comunicação entre processos

| De → Para | Mecanismo | Dados |
|-----------|-----------|-------|
| Principal → filho do heredoc | Memória herdada pelo fork | Lista de nós com os delimitadores |
| Filho do heredoc → filho de comando | `pipe()` criado pelo principal | Texto do heredoc |
| Filho de comando → filho de comando | `pipe()` criado pelo principal em `open_fds` | stdout → stdin |
| Filhos → principal | `waitpid` | Status de saída |
| Principal → filhos | Herança de fds e do `envp` original | Ambiente (sem as alterações de `export`) |
