# ADR-004: Tabela hash própria para variáveis de ambiente

- **Status:** Aceito (retroativo)
- **Data:** 2022-05-16 a 2022-05-24 (PRs #19 e #20, `branch-tables` e `hash_table`) 🟢
- **Fontes:** README ("Hash-table, onde utilizamos uma linked-list para lidar com os conflitos de key"), `src/hash_table/` 🟢

## Contexto
`export`, `unset`, `cd` (PWD/OLDPWD) e a expansão de `$VAR` precisam de um ambiente que possa ser alterado. O `envp` recebido por `main` é somente leitura na prática, e funções como `setenv` não estão entre as autorizadas pela 42.

## Decisão
Copiar o `envp` para uma **tabela hash de 149 buckets**, com hash aditivo (soma ASCII) e colisão resolvida por **encadeamento separado**. Guardar também o `$?` na chave especial `"?"` 🟢.

## Consequências
- ✅ Busca, inserção e remoção em tempo médio O(1) (na prática, o ambiente tem menos de 100 variáveis).
- ✅ Uma fonte única para `$VAR`, `$?`, `PATH`, `HOME` e `PWD`.
- ⚠️ A implementação tem defeitos na remoção (cabeça da lista e bucket com um item) e na inserção (deduplica só contra a cabeça). 🟢
- ⚠️ **O ambiente nunca volta a ser um `char **` para o `execve`**, que continua recebendo o `envp` original. Por isso `export` não chega aos filhos. Não há evidência de que isso foi intencional (ver lacuna L-03 em `domain.md`). 🟡
- ⚠️ A chave `"?"` aparece em `env` e `export`. 🟢
- A ordem de iteração é a da hash, então `export` e `env` não saem ordenados. 🟢
