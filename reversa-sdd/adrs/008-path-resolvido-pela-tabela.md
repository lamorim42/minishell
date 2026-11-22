# ADR-008: Resolução de `PATH` pela tabela, não pelo `envp`

- **Status:** Aceito (retroativo)
- **Data:** 2022-07-23 (`fix: unset no PATH`, commit `c2fcda1`) 🟢

## Contexto
Até 23/07/2022, `path_finder` lia `PATH` direto do `envp` original. Por isso `unset PATH` não tinha efeito na busca de comandos. Havia também uma checagem `is_path_ok` em `exec_path`.

## Decisão
`get_path` passou a usar `search_item(table, "PATH")`, e `is_path_ok` foi removida. Sem `PATH` na tabela, `split_path` retorna `NULL` e o comando dá `command not found` 🟢.

## Consequências
- ✅ `unset PATH` e `export PATH=...` passam a valer para a busca de comandos.
- ⚠️ A correção foi **local**: o `execve` continua recebendo `line->envp`. A mesma classe de problema (estado da tabela que não chega aos filhos) continua existindo para todas as outras variáveis. 🟢
