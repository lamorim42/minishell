# Spec Impact Matrix

> Gerado pelo **Arquiteto** (Reversa) em 2026-11-11
> Leitura: a **linha** é o componente alterado; a **coluna** é o componente impactado.
> ● = impacto direto (chama a função ou lê/escreve a mesma estrutura) · ○ = impacto indireto (contrato de dados ou ordem do pipeline) · vazio = sem impacto

## 1. Matriz componente × componente

| Alterado ↓ / Impactado → | REPL | Facade | Contador | Tokenizer | Léxico | Sintaxe | Expansor | Aspas | Lista | Heredoc | Resolvedor | Abertura fds | Executor | Ligação fds | Builtins | Ambiente | Sinais |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| **REPL** | — | ● | | | | ● | | | | | | | | | | | ● |
| **Facade** | ● | — | ● | ● | ● | ● | ● | ● | ● | ● | ● | ● | ● | | | | ● |
| **Contador** | | ○ | — | ● | ○ | | | ○ | | | | | | | | | |
| **Tokenizer** | | ○ | ● | — | ● | ○ | ● | ● | ○ | | | | | | | | |
| **Léxico** | | | | | — | ● | ● | | ● | ○ | ○ | ● | ● | ● | | | |
| **Sintaxe** | ● | ● | | | ○ | — | | | ○ | | | | | | | | |
| **Expansor** | | | | | ○ | | — | ● | ○ | | | | | | ○ | ● | |
| **Aspas** | | | | | | | ○ | — | ● | ○ | ○ | ○ | | | ○ | | |
| **Lista** | | ● | | | | | | | — | ● | ● | ● | ● | ● | ● | | |
| **Heredoc** | | ● | | | | | | | ○ | — | | | | ● | | | ● |
| **Resolvedor** | | | | | | | | | ○ | | — | | ● | | ● | ○ | |
| **Abertura fds** | | ● | | | | | | | ○ | | | — | ● | ● | | | |
| **Executor** | | ● | | | | | | | ○ | | ○ | | — | ● | ● | ● | ● |
| **Ligação fds** | | | | | | | | | ○ | ○ | | ○ | ● | — | | | |
| **Builtins** | | | | | | | | | | | ● | | ● | | — | ● | |
| **Ambiente** | ● | | | | | | ● | | | | ● | | ● | | ● | — | ● |
| **Sinais** | ● | ● | | | | | | | | ● | | | ● | | ○ | | — |
| **`t_line` (struct)** | ● | ● | ● | ● | ● | | ● | ● | ● | ● | | ● | ● | | ○ | | ● |
| **`t_pipe_list` (struct)** | | ● | | | | | | | ● | ● | ● | ● | ● | ● | ● | | |
| **`g_minishell`** | ● | | | | | | | | | | | | | | ● | | ● |

## 2. Hotspots (componentes com maior impacto)

| Componente | Impactados diretos | Por quê |
|------------|-------------------:|---------|
| Facade | 13 | Orquestra tudo; mudar a ordem quebra os contratos implícitos |
| `t_line` | 13 | Estrutura compartilhada por todos os filtros |
| Léxico (categorias) | 7 | As strings `PIPE`/`REDO`/... são o discriminador usado em todo o `exec` e nos `fd` |
| Lista (`t_pipe_list`) | 7 | Modelo de execução |
| Ambiente | 6 | `$VAR`, `PATH`, `cd`, `$?`, `export`/`unset` |

## 3. Impacto das correções sugeridas pelos bugs

| Bug | Componentes que precisam mudar | Risco de regressão |
|-----|-------------------------------|--------------------|
| BUG-01 (crash em `>`) | Abertura fds, Facade | Baixo |
| BUG-02/10 (builtin no pai, status 0) | Executor, Builtins | Médio (sinais, `$?`) |
| BUG-03 (envp) | Executor, Ambiente (gerar `char**`) | Baixo |
| BUG-04 (prefixo de builtin) | Resolvedor, Builtins | Baixo |
| BUG-05 (expansão) | Expansor | **Alto**: interage com Tokenizer e Aspas, que têm testes |
| BUG-06/07 (export, hash) | Builtins, Ambiente | Baixo |
| BUG-08/09 (redirecionamento antes do comando) | Lista ou Ligação fds | **Alto**: muda o modelo de associação |
| BUG-11/17 (aspas, tokenização) | Tokenizer, Contador, Aspas | **Alto**: DT-01 e os testes existentes |
