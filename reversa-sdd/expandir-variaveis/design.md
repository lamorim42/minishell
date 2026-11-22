# Expandir Variáveis: Design Técnico

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Interface

| Símbolo | Assinatura | Retorno | Observação |
|---------|-----------|---------|------------|
| `expand_var` | `(t_line *line, t_hash_table *table)` | `void` | Reescreve `line->tks[i]` no lugar; não altera `lex` nem `tks_nbr` 🟢 |
| `change_value` (static) | `(t_line *line, t_hash_table *table, unsigned int i)` | `void` | Expande **o primeiro** `$` de `tks[i]` 🟢 |
| `join_str_value` (static) | `(char *key, char *value, char *str)` | `char *` novo | Monta `prefixo + valor + sufixo` 🟢 |
| `join_helper` (static) | `(char *new, char *helper)` | `char *` novo | `ft_strjoin` que libera o primeiro argumento 🟢 |
| `return_var_key` | `(char *str)` | `char *` novo | Nome depois do primeiro `$` 🟢 |
| `key_len` | `(char *str, int *i, int *len)` | `void` | Avança até um delimitador; conta o próprio `$` (compensado com `len--`) 🟢 |
| `take_prefix_var` | `(char *str, unsigned int *i)` | `char *` novo ou `NULL` | Trecho antes do primeiro `$`; deixa `*i` na posição do `$` 🟢 |
| `sufix_var` | `(char *str, unsigned int *i)` | `char *` novo ou `NULL` | Trecho a partir de `*i` até o fim 🟢 |
| `search_item` | `(t_hash_table *table, char *key)` | `char *` (cópia) ou `NULL` | Comparação exata da chave 🟢 |

**Entrada:** `line->tks` (texto cru, com aspas), `line->lex` (categorias calculadas **antes** da expansão) e a tabela de ambiente 🟢.
**Saída:** `line->tks` com as referências substituídas. As aspas continuam no texto e são removidas depois por `clean_tokens` 🟢.

## Fluxo Principal
1. `exec_pipe_line` chama `expand_var(line, *table)` logo depois de a gramática ser aceita (`src/facade.c:36`) 🟢
2. Para cada índice `i` até `lex[i] == NULL` (`src/exec/expand_var.c:24-30`) 🟢:
   - se `ft_strcmp_len(lex[i], "VAR")` **e** `tks[i][0] != '\''`:
     - enquanto `tks[i]` contém `$`: `change_value(line, table, i)`
3. `change_value` (`src/exec/expand_var.c:33-55`) 🟢:
   1. `key = return_var_key(tks[i])`
   2. `value = search_item(table, key)`
   3. `value != NULL` → `tks[i] = join_str_value(key, value, tks[i])`; libera o antigo e `value`
   4. `value == NULL` → `tks[i] = ""` (token inteiro descartado); como `""` não tem `$`, o laço do passo 2 termina
   5. libera `key`
4. Depois de todos os tokens, `exec_pipe_line` chama `clean_tokens` (`src/facade.c:37`) 🟢

### Algoritmo `return_var_key`
```
i = 0
enquanto str[i]:
  se str[i] == '$':
     inicio = i + 1
     len = 0
     enquanto str[i] e str[i] não está em "\"',:./[]{}=-+*!@# ":   # o '$' entra na contagem
        len++; i++
     len--                                                        # descarta o '$'
     pare
  i++
retorna substr(str, inicio, len)
```
Consequência: um segundo `$` não interrompe o nome (`HOME$USER`) 🟢.

### Algoritmo `join_str_value`
```
se strlen(key) + 1 == strlen(str):      # o token é exatamente "$KEY"
   retorna strdup(value)
i = 0
prefixo = take_prefix_var(str, &i)      # str[0..i), i fica no '$'
novo = prefixo ? prefixo + value : strdup(value)
i += strlen(key) + 1                    # pula "$KEY"
se str[i]: novo = novo + sufix_var(str, &i)
retorna novo
```
O atalho do início compara só tamanhos. Ele é correto porque o corte de tokens garante que um token com prefixo é sempre mais longo que `$KEY` 🟡.

### Atualização de `$?` (produtor, fora desta unit)
`update_status_code` grava a chave `?` depois de cada execução (`src/exec/exec_list.c:54-69`) 🟢:

| Condição de `status_code` | Valor gravado |
|---------------------------|---------------|
| `WIFEXITED` | `WEXITSTATUS` |
| `== 1` | `1` |
| `WIFSIGNALED` | `WTERMSIG + 128` |
| outro | o próprio valor |

A expansão só lê a chave; ela é tratada como qualquer outra variável 🟢.

## Fluxos Alternativos
- **Token que começa com `'`:** ignorado inteiro, mesmo que tenha `"$X"` depois: `'a'"$HOME"` → `a$HOME` 🟢 (observado). ⚠️ O bash dá `a/home/u`
- **Variável inexistente:** token inteiro vira `""` (RX-02) 🟢 (observado)
- **Valor vazio:** `join_str_value` produz prefixo + sufixo; o token pode ficar `""` 🟢 (observado)
- **Valor com `$`:** o `while` do passo 2 volta a expandir o resultado. Se o valor sempre recria um `$`, o laço não termina (RX-06) 🟢 (observado)
- **Nome vazio** (`"$"`, `${X}`, `$/`): `key = ""`, `search_item` não acha nada e o token vira `""` 🟢 (observado)
- **`$` sozinho:** lexema `WORD`, nunca chega a `change_value` 🟢

## Dependências
- **hash_table:** `search_item` (leitura), mantida pela unit `gerenciar-variaveis-de-ambiente`
- **analisar-linha-de-comando:** fornece `tks` e `lex`; consome o resultado em `clean_tokens`
- **executar-comando-simples / executar-pipeline:** `exec_list` produz o valor de `?`
- **Libft:** `ft_strchr`, `ft_substr`, `ft_strjoin`, `ft_strdup`, `ft_strlen`

## Decisões de Design Identificadas

| Decisão | Evidência no código | Confiança |
|---------|---------------------|-----------|
| A expansão age sobre o texto do token, não sobre uma árvore de palavras; o contexto de aspas é aproximado pelo primeiro caractere | `src/exec/expand_var.c:26` | 🟢 |
| O lexema é calculado antes da expansão e não é recalculado | `src/facade.c:22-36` | 🟢 |
| `$?` é guardado na própria tabela de ambiente, como uma variável comum de nome `?` | `src/hash_table/create_table.c:55`, `src/exec/exec_list.c:66-67` | 🟢 |
| O valor é expandido de novo enquanto houver `$`, sem limite | `src/exec/expand_var.c:27` | 🟢 |
| Variável inexistente descarta o token inteiro, em vez de só o trecho | `src/exec/expand_var.c:48-53` | 🟢 |
| Os casos de aspas misturadas foram corrigidos um a um (commits `fix: single_quouts expand`, `fix: echo 'oi'$USER'oi'`) | histórico Git | 🟡 |

## Estado Interno
Sem estado próprio. Lê a tabela (sessão) e reescreve `line->tks` (linha). Cada iteração de `change_value` aloca um novo `tks[i]` e libera o anterior 🟢.

## Observabilidade
Nenhuma mensagem nem log. Um nome inválido ou inexistente produz um token vazio em silêncio 🟢.

## Riscos e Lacunas
- 🟢 Loop infinito com valor auto-referente (`A='$A'`, `B='-$B'`). Um valor assim só entra pela herança do ambiente, porque o próprio `export` expande o `$` antes de gravar
- 🟢 `$` não delimita o nome, então `$A$B` falha
- 🟢 `$?abc` procura a chave `?abc`; no bash, `$?` tem sempre um caractere
- 🟢 Tokens vazios chegam como argumento `""` aos comandos externos (`ls $NOPE` → `ls: cannot access ''`) (observado na revisão de 2026-11-22); se o vazio é o **primeiro** argumento, a linha é ignorada (BUG-20)
- 🟡 Nenhum `malloc` (`ft_substr`, `ft_strjoin`, `ft_strdup`) tem o retorno testado
- ✅ L-07 (decidida em 2026-11-22): documentar o as-is. Nota para uma eventual reimplementação corretiva, fora do escopo atual: preservar ou corrigir BUG-05, RX-06, a reexpansão de valores e a remoção das aspas vindas dos valores
- ✅ L-01 (respondida em 2026-11-22): o alvo é o **mínimo exigido pelo subject** da 42; *word splitting*, `${}`, `$$` e a proteção por aspas simples em qualquer posição ficam **fora do escopo** 🟢
