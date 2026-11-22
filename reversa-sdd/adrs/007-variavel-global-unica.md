# ADR-007: Uma única variável global (`g_minishell`)

- **Status:** Aceito (retroativo)
- **Data:** 2022-07-23 (`putting global conditions`), na véspera da entrega 🟢

## Contexto
Handlers de sinal não recebem parâmetros de contexto, e alguns builtins precisam atualizar `status_code` sem receber a `t_line`. A norma da 42 permite no máximo uma variável global.

## Decisão
Criar `struct s_minishell { t_line *line; t_hash_table *table; } g_minishell;` em `main.c` e declará-la `extern` em `signals*.c` e `builtins/*.c` 🟢.

## Consequências
- ✅ Cumpre a norma (uma única global).
- ✅ `sig_here` consegue liberar tudo antes do `exit(130)`, e os builtins conseguem gravar o status.
- ⚠️ A global guarda um **ponteiro para a tabela**, copiado uma vez. Se a tabela fosse realocada, ficaria obsoleta (hoje isso não acontece). 🟢
- ⚠️ Os handlers acessam estado complexo e chamam funções que não são *async-signal-safe*. 🟡
