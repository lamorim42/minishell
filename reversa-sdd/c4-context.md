# C4 Nível 1: Contexto

> Gerado pelo **Arquiteto** (Reversa) em 2026-11-11 · 🟢 salvo indicação em contrário

```mermaid
flowchart TB
    user(["👤 Usuário de terminal<br/><i>pessoa que digita comandos</i>"])
    shell["🐱 <b>minishell / MiauShell</b><br/>Interpretador de comandos interativo<br/><i>C · clang · GNU Readline</i>"]
    os[["🖥️ Kernel POSIX / Linux<br/><i>fork · execve · pipe · dup2 · signal · open · chdir</i>"]]
    progs[["⚙️ Programas externos<br/><i>/bin/ls, /usr/bin/cat, ...</i>"]]
    fs[("📁 Sistema de arquivos<br/><i>arquivos de redirecionamento, diretórios do PATH</i>")]
    term[["⌨️ Terminal / TTY<br/><i>teclado, sinais Ctrl-C, Ctrl-D, Ctrl-\</i>"]]

    user -- "digita linhas, envia sinais" --> term
    term -- "stdin, SIGINT, SIGQUIT" --> shell
    shell -- "prompt, saída dos builtins, erros" --> term
    shell -- "chamadas de sistema" --> os
    os -- "cria processos e executa" --> progs
    progs -- "stdout e stderr herdados ou redirecionados" --> term
    shell -- "open, access, chdir, getcwd" --> fs
    progs -- "leem e escrevem" --> fs
```

| Elemento | Tipo | Descrição |
|----------|------|-----------|
| Usuário de terminal | Pessoa | Única persona. Não há papéis nem autenticação |
| minishell | Sistema (foco) | Processo único e interativo, que lê do stdin com readline |
| Kernel POSIX | Sistema externo | Toda a "integração" se dá por chamadas de sistema |
| Programas externos | Sistema externo | Qualquer executável alcançável por caminho ou pelo `PATH` |
| Sistema de arquivos | Armazenamento | Alvos de `<`, `>`, `>>`, diretórios do `PATH` e do `cd` |
| Terminal | Dispositivo | Origem da entrada e dos sinais. O shell assume que é um TTY interativo 🟡 |

**Integrações de rede, APIs, filas ou banco de dados:** nenhuma 🟢.
