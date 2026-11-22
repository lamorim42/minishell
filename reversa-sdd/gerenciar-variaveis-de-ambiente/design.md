# Gerenciar Variáveis de Ambiente: Design Técnico

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Interface

### Tabela hash (`src/hash_table/`)

| Símbolo | Assinatura | Retorno | Observação |
|---------|-----------|---------|------------|
| `create_table` | `(int size)` | `t_hash_table *` | `size = SIZE_TABLE = 149`; buckets zerados 🟢 |
| `hash_function` | `(char *str)` | `unsigned long` | `Σ bytes mod 149` 🟢 |
| `create_item` | `(char *key, char *value)` | `t_hash_item *` | Duplica chave e valor 🟢 |
| `hash_insert` | `(t_hash_table **t, char *key, char *value)` | `void` | Atualiza se a **cabeça** do bucket tem a chave; senão encadeia no fim 🟢 |
| `handle_collision` | `(t_hash_table **t, t_hash_item *item, int index)` | `void` | Anexa no fim da lista 🟢 |
| `search_item` | `(t_hash_table *t, char *key)` | `char *` (cópia) ou `NULL` | Igualdade exata (`ft_strncmp` com `len + 1`) 🟢 |
| `table_delete` | `(t_hash_table **t, char *key)` | `void` | Bucket com 1 item: apaga **sem conferir a chave**; com mais: `delete_node` 🟢 |
| `delete_node` | `(t_hash_table **t, t_hash_item *head, char *key)` | `void` | Procura a chave; só remove se **não** for a cabeça 🟢 |
| `print_table` | `(t_hash_table **t)` | `void` | `printf("%s=%s\n")` na ordem dos buckets 🟢 |
| `free_item` / `free_table` | — | `void` | 🟢 |

Estruturas: `t_hash_table { int size; int count; t_hash_item **item; }` e `t_hash_item { char *key; char *value; t_hash_item *next; }` 🟢.

### Builtins

| Símbolo | Assinatura | Observação |
|---------|-----------|------------|
| `export_builtin` | `(t_pipe_list *node, t_hash_table **table)` | 🟢 |
| `is_args_ok` | `(char *arg)` → bool | `ft_isalpha(arg[0])` 🟢 |
| `insert_var_in_hash` | `(char *arg, t_hash_table **table)` | `ft_split(arg, '=')` e "remontagem" do valor 🟢 |
| `building_var` | `(char **split, char *str, char *s)` | `split[1] = str + s` 🟢 |
| `insert_simple_var_in_hash` | `(t_hash_table **table, char **split)` | Novo → `hash_insert`; existente → `update_var_env` 🟢 |
| `update_var_env` | `(t_hash_table **table, char *key, char *val)` | `table_delete` (se existir) + `hash_insert` 🟢 |
| `unset_builtin` | `(t_pipe_list *node, t_hash_table **table)` | 🟢 |
| `env_builtin` | `(t_pipe_list *node, t_hash_table **table)` | 🟢 |

## Fluxo Principal

### `export` (`src/builtins/export.c:27-43`)
```
para cada args[i], i ≥ 1:
  se primeiro caractere é letra e contém '=': insert_var_in_hash(args[i])
  senão se primeiro caractere não é letra: status_code = 1; "<arg>: not a valid identifier"
  # letra sem '=': nada
se não há args[1]: print_export   # "declare -x K=\"V\"" por bucket
```

### `insert_var_in_hash` (`src/builtins/builtins_utils.c:60-84`)
```
s = ft_split(arg, '=')                 # "E=b=c" → ["E","b","c"]; "D=" → ["D"]
se len(s) > 2:
  para i = 1 .. fim:
    i ímpar: s[1] = s[i] + "="         # i=1: "b="
    i par:   s[1] = s[1] + s[1]        # i=2: "b=b="   (deveria acrescentar s[i])
se s[1] existe: insert_simple_var_in_hash(s)     # "D=" não tem s[1] → ignorado
```
Esse é o mecanismo do BUG-06 🟢.

### `unset` (`src/builtins/unset.c:17-39`)
```
para cada args[i]:
  se primeiro caractere é letra:
     se search_item(args[i]) existe: table_delete(args[i])
  senão: status_code = 1; "<arg>: not a valid identifier"
```

### `table_delete` + `delete_node` (`src/hash_table/delete_node.c:17-48`)
```
cabeça = bucket[hash(key)]
se cabeça->next == NULL: free(cabeça); bucket = NULL         # sem conferir a chave
senão:
  anterior = NULL; atual = cabeça
  enquanto atual e atual->key != key: anterior = atual; atual = atual->next
  se atual e anterior: anterior->next = atual->next; free(atual)
  # se a chave é a cabeça (anterior == NULL): nada acontece → BUG-07
```

### `env` (`src/builtins/env.c:17-26`)
Com argumento: `status_code = 127` e mensagem. Sem argumento: `print_table` 🟢.

## Fluxos Alternativos
- **Atualização de chave existente na cabeça de uma lista:** `update_var_env` → `table_delete` não remove (BUG-07) → `hash_insert` encontra a chave na cabeça e atualiza o valor. O resultado final é correto 🟢
- **Atualização de chave existente fora da cabeça:** `delete_node` remove → `hash_insert` anexa de novo no fim. Correto 🟢
- **`hash_insert` de chave existente fora da cabeça, sem `table_delete` antes:** cria **duplicata** no bucket. Hoje nenhum chamador faz isso, porque `update_var_env` sempre apaga antes 🟡
- **Atualização de `?`** (`update_status_code`): `table_delete("?")` + `hash_insert("?", status)`. Funciona pelas mesmas razões 🟢

## Dependências
- **Libft:** `ft_split`, `ft_strjoin`, `ft_strdup`, `ft_isalpha`, `ft_strncmp`
- **iniciar-sessao-e-ler-comando:** `population_hash_table` na inicialização
- **expandir-variaveis, executar-comando-simples, navegar-diretorios:** leitores da tabela (`search_item`)
- **executar-comando-simples:** `exec_builtins` despacha; o builtin só altera o shell se for o último nó

## Decisões de Design Identificadas

| Decisão | Evidência no código | Confiança |
|---------|---------------------|-----------|
| Ambiente próprio em tabela hash com encadeamento (ADR-004) | `src/hash_table/` | 🟢 |
| `$?` como chave `?` na mesma tabela | `src/hash_table/create_table.c:55` | 🟢 |
| Tamanho fixo (149), sem *rehash* | `includes/minishell.h` | 🟢 |
| `execve` não recebe a tabela (os filhos usam o `envp` original) | `src/exec/exec_path.c:34` | 🟢 |
| Erros de builtin gravados em `g_minishell.line->status_code` | `src/builtins/export.c:36`, `unset.c:33`, `env.c:21` | 🟢 |

## Estado Interno
A tabela vive no heap durante a sessão inteira, apontada por `main` e por `g_minishell.table`. `count` é mantido, mas não é usado para decisões 🟢.

## Observabilidade
Mensagens em stderr: `<arg>: not a valid identifier` e `<arg>: No such file or directory` 🟢.

## Riscos e Lacunas
- 🟢 BUG-06: valores com `=` e valores vazios
- 🟢 BUG-07: `unset` da cabeça de colisão; `table_delete` apaga sem conferir a chave em bucket com 1 item
- 🟢 EX-02: validação só do primeiro caractere
- 🟢 EN-01 e EX-05: `?` exposto em `env` e `export`
- 🟡 A função de hash (soma dos bytes) gera colisões entre anagramas (`AB`/`BA`) e torna o BUG-07 comum
- ✅ L-03 (respondida em 2026-11-22): é um **bug** (BUG-03), confirmado pelo autor; documentado como comportamento atual (L-07)
- ✅ L-07 (decidida em 2026-11-22): documentar o as-is. Nota para uma eventual reimplementação corretiva, fora do escopo atual: corrigir a tabela (`delete` da cabeça) muda o comportamento observável só do `unset`
