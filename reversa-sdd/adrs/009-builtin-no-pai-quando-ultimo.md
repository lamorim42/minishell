# ADR-009: Builtin roda no processo pai quando é o último nó

- **Status:** Aceito (retroativo, intenção inferida)
- **Fontes:** `src/exec/exec_list.c:30-32`, commit `feat: Status code modifications` (2022-07-18) 🟢 código / 🟡 intenção

## Contexto
`cd`, `export`, `unset` e `exit` só fazem sentido se alterarem o estado do **próprio shell**. Dentro de um pipeline, o bash executa cada parte em um subshell.

## Decisão (como implementada)
Um builtin é executado no pai quando o nó dele é o **último da lista** (`temp->next == NULL`). Em qualquer outro caso, é executado em um filho, que sempre sai com 0 🟢.

## Consequências
- ✅ `cd dir`, `export X=1` e `exit` sozinhos funcionam.
- ⚠️ O critério deveria ser "linha com um único comando e sem pipe". Como a regra usa "último nó", aparecem três problemas 🟢:
  - `ls | exit` encerra o shell, e `ls | cd /` muda o diretório.
  - `cd /tmp > f` roda em um filho e não muda o diretório.
  - `> f echo` roda no pai e ignora o redirecionamento.
- ⚠️ Builtins executados em filhos perdem o código de erro (sempre 0).
