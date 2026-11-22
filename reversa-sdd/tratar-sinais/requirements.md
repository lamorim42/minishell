# Tratar Sinais

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash
> Casos "(observado)" foram executados contra o `main.c` legado compilado na scratchpad e conduzido por um pseudo-terminal (`pty`), enviando `\x03` (Ctrl-C) e `\x1c` (Ctrl-\). O `readline` substituto não redesenha o prompt, então a parte visual do prompt é 🟢 só pelo código. O repositório não foi alterado.

## Visão Geral
Define como o shell reage a Ctrl-C (SIGINT) e Ctrl-\ (SIGQUIT) em cada fase: prompt, leitura de heredoc e execução de comandos. O comportamento troca conforme a fase, instalando handlers diferentes com `signal()` 🟢.

## Responsabilidades
- No prompt: Ctrl-C mostra um prompt novo e limpo; Ctrl-\ é ignorado 🟢
- Durante o fork do heredoc: o shell ignora os dois sinais 🟢
- No filho do heredoc: Ctrl-C encerra a leitura com 130 🟢
- Durante a execução: o shell repassa o efeito aos filhos e mostra uma nova linha (Ctrl-C) ou `Quit` (Ctrl-\) 🟢
- Registrar o status 130/131 dos filhos mortos por sinal 🟢

## Regras de Negócio

### Prompt (`signals`)
- SG-01: Ctrl-C → `\n` em stderr, linha atual apagada (`rl_replace_line("", 0)`) e prompt redesenhado 🟢 (pelo código)
- SG-02: Ctrl-C no prompt **não altera `$?`**: depois de `ls /nope`, Ctrl-C e `echo $?` → `2` 🟢 (observado). ⚠️ O bash grava 130
- SG-03: Ctrl-\ no prompt é ignorado 🟢 (observado), igual ao bash
- RN-02: Ctrl-D é EOF, não sinal (ver `iniciar-sessao-e-ler-comando`) 🟢

### Heredoc (`signal_ignore` + `signal_here`)
- SG-04: entre o fork do heredoc e o próximo comando, o shell ignora SIGINT e SIGQUIT 🟢
- SG-05: no filho do heredoc, Ctrl-C → `\n`, libera a memória e sai com 130; Ctrl-\ é ignorado 🟢
- BUG-15: o shell não confere esse status, executa o comando com stdin vazio e `$?` não vira 130 (ver `ler-heredoc`) 🟢 (observado)

### Execução (`signals_parent`, instalado antes de cada `fork` de comando)
- SG-06: Ctrl-C durante um comando externo → o filho morre (ação padrão), o shell escreve `\n` em stderr e `$?=130`: `sleep 5` + Ctrl-C → `130`; `cat | cat` + Ctrl-C → `130` 🟢 (observado), igual ao bash
- SG-07: Ctrl-\ durante um comando externo → o filho morre com SIGQUIT, o shell escreve `Quit\n` em stderr e `$?=131` 🟢 (observado). ⚠️ O bash escreve `Quit (core dumped)`
- SG-08: os filhos que fazem `execve` voltam ao comportamento padrão dos sinais, porque handlers instalados não sobrevivem ao `execve` 🟢
- SG-09: um builtin executado em filho herda `sig_parent`/`sig_write` e não morre com Ctrl-C 🟡
- SG-10: `$?` vem de `waitpid`: `WIFSIGNALED` → `128 + sinal` (`update_status_code`) 🟢
- ✅ L-07 (decidida em 2026-11-22, [Revisão]): **documentar o comportamento atual**. As ⚠️ DIVERGÊNCIAS desta unit são o comportamento especificado, não defeitos a corrigir. (Pendência original: SG-02 e BUG-15)

## Requisitos Funcionais

| ID | Requisito | Prioridade | Critério de Aceite |
|----|-----------|-----------|-------------------|
| RF-01 | Ctrl-C no prompt mostra um prompt novo sem encerrar o shell | Must | Ctrl-C → novo `miau> ` |
| RF-02 | Ctrl-\ no prompt não faz nada | Must | Ctrl-\ → nenhuma saída |
| RF-03 | Ctrl-C durante um comando o interrompe e `$?=130` | Must | `sleep 5` + Ctrl-C; `echo $?` → `130` |
| RF-04 | Ctrl-\ durante um comando o interrompe, mostra `Quit` e `$?=131` | Must | `sleep 5` + Ctrl-\ → `Quit`; `echo $?` → `131` |
| RF-05 | Ctrl-C no heredoc encerra a leitura | Must | Ver `ler-heredoc` RF-07 |

## Requisitos Não Funcionais

| Tipo | Requisito inferido | Evidência no código | Confiança |
|------|--------------------|---------------------|-----------|
| Segurança de sinais | Handlers chamam funções que não são *async-signal-safe* (`printf`, `rl_*`, `free`) | `src/signals_utils.c:23-55`, `src/signals.c:17-24` | 🟢 |
| Estado global | Os handlers acessam a linha e a tabela pela global única `g_minishell` (ADR-007) | `src/signals.c:21`, `src/signals_utils.c:28-30` | 🟢 |
| Portabilidade | Usa `signal()` (semântica BSD com `SA_RESTART` na glibc), não `sigaction` | `src/signals*.c` | 🟢 |

## Critérios de Aceitação

```gherkin
Funcionalidade: Sinais

  Cenário: Ctrl-C no prompt
    Dado o prompt com "abc" digitado
    Quando o usuário envia Ctrl-C
    Então uma nova linha é escrita
    E o prompt aparece vazio

  Cenário: Ctrl-\ no prompt
    Quando o usuário envia Ctrl-\
    Então nada acontece

  Cenário: Interromper um comando
    Dado que "sleep 5" está executando
    Quando o usuário envia Ctrl-C
    Então o comando termina
    E $? é 130

  Cenário: Quit em um comando
    Dado que "sleep 5" está executando
    Quando o usuário envia Ctrl-\
    Então "Quit" é escrito em stderr
    E $? é 131

  @divergente
  Cenário: Ctrl-C no prompt não altera $? (SG-02)
    Dado que o último status é 2
    Quando o usuário envia Ctrl-C no prompt
    E depois digita "echo $?"
    Então a saída é "2"
```

## Prioridade (MoSCoW)

| Requisito | MoSCoW | Justificativa |
|-----------|--------|---------------|
| RF-01 a RF-05 | Must | Ctrl-C, Ctrl-D e Ctrl-\ são obrigatórios no subject |

## Rastreabilidade de Código

| Arquivo | Função | Cobertura |
|---------|--------|-----------|
| `src/signals_utils.c` | `signals`, `sig_handler`, `signal_here`, `sig_here` | 🟢 |
| `src/signals.c` | `signal_ignore`, `signals_parent`, `sig_parent`, `sig_write` | 🟢 |
| `src/main.c` | chamada de `signals` a cada iteração | 🟢 |
| `src/facade.c` | `signal_ignore` / `signal_here` em volta do fork do heredoc | 🟢 |
| `src/exec/init_fork.c` | `signals_parent` antes de cada fork | 🟢 |
| `src/exec/exec_list.c` | `update_status_code` (128 + sinal) | 🟢 |
