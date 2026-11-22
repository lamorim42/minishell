# Gerenciar Variáveis de Ambiente: Tarefas de Implementação

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Pré-requisitos
- [ ] Libft com `ft_split`, `ft_strjoin`, `ft_strdup`, `ft_isalpha`
- [ ] Unit `executar-comando-simples` com o despacho de builtins (`exec_builtins`)
- [x] Decisões de L-03 (ambiente dos filhos: **bug**, respondida) e L-07 (BUG-06, BUG-07, validação, `?` visível) → **L-07 decidida em 2026-11-22: documentar o as-is**

## Tarefas

- [ ] T-01, Implementar a tabela: `create_table`, `hash_function`, `create_item`, `hash_insert`, `handle_collision`, `search_item`, `free_item`, `free_table`
  - Origem no legado: `src/hash_table/*.c`
  - Critério de pronto: inserir 500 chaves e encontrar todas; `search_item` devolve cópia
  - Confiança: 🟢

- [ ] T-02, Implementar `table_delete` / `delete_node` removendo também a cabeça da lista e conferindo a chave em buckets de 1 item
  - Origem no legado: `src/hash_table/delete_node.c:17-48` (BUG-07)
  - Critério de pronto (corrigido): `export A=1 kk=2`; `unset A` → `A` removida. Critério de pronto (as-is): `A` continua
  - Confiança: 🟢
  - Nota: depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-03, Implementar `export K=V` (criação e atualização via `update_var_env`), com o laço sobre todos os argumentos
  - Origem no legado: `src/builtins/export.c:27-43`, `src/builtins/builtins_utils.c:60-102`, `src/builtins/cd.c:42-51`
  - Critério de pronto: cenários "Criar e ler" e "Atualizar"
  - Confiança: 🟢

- [ ] T-04, Tratar valores com `=` e valores vazios
  - Origem no legado: `src/builtins/builtins_utils.c:60-84` (BUG-06)
  - Critério de pronto (corrigido): `export E=b=c` → `b=c`; `export D=` → `D` vazia. Critério de pronto (as-is): `b=b=` e `D` ausente
  - Confiança: 🟢
  - Nota: separar só no **primeiro** `=` resolve os dois; depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-05, Validar identificadores (`is_args_ok`) e informar `not a valid identifier` com `$?=1`
  - Origem no legado: `src/builtins/builtins_utils.c:53-58`, `src/builtins/export.c:34-38`
  - Critério de pronto: `export 1A=2` e `unset 9` → mensagem e `$?=1`
  - Confiança: 🟢
  - Nota: aceitar `_` e validar todos os caracteres (`[A-Za-z_][A-Za-z0-9_]*`) alinha com o bash; depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-06, Implementar `export` sem argumentos (`declare -x K="V"`)
  - Origem no legado: `src/builtins/export.c:54-86`
  - Critério de pronto: a saída contém uma linha por variável
  - Confiança: 🟢
  - Nota: ordenar e esconder `?` alinha com o bash; depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-07, Implementar `unset` com validação
  - Origem no legado: `src/builtins/unset.c:17-39`
  - Critério de pronto: cenário "Remover"; `unset ?` → `not a valid identifier`
  - Confiança: 🟢

- [ ] T-08, Implementar `env` (sem argumento: `print_table`; com argumento: `No such file or directory`, 127)
  - Origem no legado: `src/builtins/env.c:17-26`, `src/hash_table/print_table.c`
  - Critério de pronto: cenário "env com argumento"
  - Confiança: 🟢
  - Nota: esconder `?` (EN-01) depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-09, (Correção do BUG-03, só numa reimplementação corretiva) gerar um `envp` a partir da tabela para o `execve`
  - Origem no legado: `src/exec/exec_path.c:34` (BUG-03)
  - Critério de pronto: `export ZZ=1`; `sh -c 'echo $ZZ'` → `1`
  - Confiança: 🟢
  - Nota: L-03 respondida: é bug; fora do escopo atual (L-07 = documentar)

## Tarefas de Teste

- [ ] TT-01, Testes unitários da tabela: inserção, busca, atualização, remoção da cabeça, do meio e do fim de uma lista de colisão (chaves `A`/`kk`, soma 65)
- [ ] TT-02, Cenários de `requirements.md`
- [ ] TT-03, Cenários `@divergente` (BUG-06, BUG-07, EN-01) (L-07 = as-is: estes cenários são o comportamento esperado)
- [ ] TT-04, Valgrind: 1.000 `export`/`unset` alternados sem vazamento

## Ordem Sugerida
1. T-01 → T-02 (estrutura; todas as outras units dependem dela)
2. T-03 → T-04 → T-05 (export)
3. T-06 → T-07 → T-08 (listagem e remoção)
4. T-09 só numa reimplementação corretiva

## Lacunas Pendentes (🔴)
- ✅ L-03 (respondida em 2026-11-22): é um **bug** (BUG-03), confirmado pelo autor; documentado como comportamento atual (L-07)
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). Só para uma eventual reimplementação corretiva: BUG-06, BUG-07, validação de identificadores (EX-02), `export K` sem `=` (EX-03), `?` visível (EX-05, EN-01)
