# Analisar Linha de Comando: Design Técnico

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Interface

| Símbolo | Assinatura | Retorno | Observação |
|---------|-----------|---------|------------|
| `building_tokens` | `(t_line *line)` | `void` | Preenche `tks_nbr`, `tks` e `lex` 🟢 |
| `count_tks` | `(char *line)` | `int` | Número de tokens; reimplementa as regras do `tokenizer` 🟢 |
| `tokenizer` | `(t_line *line)` | `char **` terminado em `NULL` | Lê `line->str`; retorna `NULL` se o `malloc` falhar 🟢 |
| `worddup` (static) | `(char **s)` | `char *` | Copia um token e avança `*s` 🟢 |
| `make_red_len` / `make_quots_len` / `make_word_len` (static) | `(char *str, int *len)` | `void` | Calculam o tamanho do token a partir de `str` 🟢 |
| `lexical_analyzer` | `(t_line *line)` | `char **` terminado em `NULL` | Um lexema por token, mesmo índice 🟢 |
| `find_lexema` (static) | `(t_line *line, char **lex, int i)` | `void` | Grava `lex[i]` 🟢 |
| `sintax_analysis` | `(char **lex)` | `int` (`TRUE`/`FALSE`) | Só olha `lex`, nunca o texto 🟢 |
| `free_sintax` | `(t_line *line)` | `void` | Imprime `Syntax ERROR!` e libera `tks`, `lex`, `str` 🟢 |
| `clean_tokens` | `(t_line *line)` | `char **` terminado em `NULL` | Lê `line->tks` (já expandido) 🟢 |
| `clean_quots` / `clean_in_middle` (static) | `(char *tks)` | `char *` novo | 🟢 |
| `count_quots` | `(char *tks)` | `int` | Conta `"` e `'` 🟢 |
| `ft_strcmp_len` | `(char *s1, char *s2)` | `char` (bool) | Igualdade exata; usado por `expand_var` para testar `"VAR"` 🟢 |

**Entrada:** `line->str`, com aspas balanceadas e sem ser comentário (garantido por `main_loop`) 🟢.

**Saída (campos de `t_line`):**

| Campo | Conteúdo | Produzido em |
|-------|----------|--------------|
| `tks_nbr` | quantidade de tokens | `building_tokens` |
| `tks[]` | texto cru de cada token, com aspas | `tokenizer` |
| `lex[]` | `"WORD"`, `"VAR"`, `"PIPE"`, `"REDI"`, `"REDO"`, `"REDA"` ou `"HERE"` | `lexical_analyzer` |
| `ctks[]` | texto de `tks[]` (já expandido) sem aspas | `clean_tokens` |

Os três arrays são **paralelos**: o índice `i` identifica o mesmo token em todos (ADR-003) 🟢.

## Fluxo Principal
1. `facade` verifica `str != NULL && ft_strlen(str) > 0` (`src/facade.c:20`) 🟢
2. `building_tokens` (`src/tokens/tokenizer_utils.c:15-20`) 🟢:
   1. `tks_nbr = count_tks(str)`
   2. `tks = tokenizer(line)`: chama `count_tks` **de novo** para dimensionar o array
   3. `lex = lexical_analyzer(line)`: aloca `tks_nbr + 1`
3. `sintax_analysis(lex)` (`src/facade.c:23`) 🟢:
   - `FALSE` → `free_sintax(line)` e fim da linha
   - `TRUE` → `exec_pipe_line`
4. `exec_pipe_line` chama `expand_var(line, table)` (unit `expandir-variaveis`), que reescreve `tks[i]` no lugar sem tocar em `lex` 🟢
5. `ctks = clean_tokens(line)` (`src/facade.c:37`) 🟢
6. `creat_list_cmd` consome `lex` (para agrupar) e `ctks` (para os argumentos) (unit `executar-comando-simples`) 🟢

### Algoritmo de corte (`tokenizer` + `worddup`)
```
ptr = str
enquanto *ptr:
  se *ptr == ' ': ptr++; continue
  len = 0
  se *ptr em "|><":   make_red_len
  senão se *ptr em "\"'": make_quots_len
  senão:              make_word_len
  tks[i++] = cópia de ptr[0..len); ptr += len
tks[i] = NULL
```

```
make_red_len(str, len):
  op = str[len]; len++
  se op == '>' e str[len] == '>': len++
  se op == '<' e str[len] == '<': len++

make_quots_len(str, len):
  q = str[len]; len++
  avança até str[len] == q ou fim; se achou q: len++
  enquanto str[len] e str[len] não está em "|><$ ":
     se str[len] é aspa: make_quots_len(str, len); retorna   # absorve outro trecho
     len++

make_word_len(str, len):
  inicio = str[len]
  enquanto str[len] e str[len] não está em "|>< ":
     se str[len] é aspa:
        se inicio != '$': make_quots_len(str, len); retorna   # absorve (AT-05)
        senão: retorna                                       # corta (AT-06)
     len++
```
Note a assimetria: `make_quots_len` para em `$`, `make_word_len` não 🟢.

### Algoritmo de contagem (`count_tks`)
Mesma máquina de estados, mas incrementa um contador em vez de copiar. Quando uma palavra continua em um trecho entre aspas, o contador é incrementado pela chamada recursiva e decrementado antes dela, para que o token continue contando como um só (`src/tokens/count_tks.c:69,86`) 🟢.

### Classificação (`find_lexema`)
Avaliada em ordem, a primeira que casar vence (`src/tokens/lexical_analyzer.c:35-50`) 🟢:

| Ordem | Teste | Lexema |
|-------|-------|--------|
| 1 | `strncmp(t, "\|", 1) == 0` | `PIPE` |
| 2 | `t == ">>"` | `REDA` |
| 3 | `t == "<<"` | `HERE` |
| 4 | `strncmp(t, ">", 1) == 0` | `REDO` |
| 5 | `strncmp(t, "<", 1) == 0` | `REDI` |
| 6 | `strchr(t, '$') && strlen(t) > 1` | `VAR` |
| 7 | — | `WORD` |

Como o corte nunca produz operador junto com texto, os testes por prefixo (1, 4, 5) só casam com o operador sozinho 🟢.

### Gramática (`sintax_analysis`)
```
se lex[0] existe e é PIPE: FALSE                       # G-01
para cada i:
  se lex[i] == PIPE e lex[i+1] == PIPE: FALSE           # G-02
  se lex[i] em {REDO, REDA, REDI, HERE}
     e (lex[i+1] == NULL ou lex[i+1] != WORD): FALSE    # G-03
TRUE
```
`PIPE` seguido de `NULL` não é testado, por isso `ls |` passa (RN-12) 🟢. As comparações usam `ft_strncmp(..., 4)` 🟢.

### Remoção de aspas (`clean_tokens`)
```
para cada tks[i]:
  sem aspas                         → strdup
  começa e termina com '"'          → ft_strtrim(t, "\"")   # tira todas as " das pontas
  começa e termina com '\''         → ft_strtrim(t, "'")
  caso contrário                    → clean_in_middle: copia sem nenhuma aspa
ctks[i] = NULL
```
`ft_strtrim` remove qualquer sequência de `"` nas pontas, não só uma aspa: `"a"b"c"` → `a"b"c` e `"a"""` → `a` 🟢.

## Fluxos Alternativos
- **Linha só com espaços:** `count_tks` = 0; `tks` e `lex` = `{NULL}`; a gramática retorna `TRUE` e o pipeline segue sem nós 🟢
- **Erro de sintaxe:** `free_sintax` faz `printf("Syntax ERROR!\n")`, `ft_free_arr(tks)`, `ft_free_arr(lex)`, `free(str)`; `status_code` não é alterado e `add_history` não é chamado (`src/free.c:47-56`) 🟢
- **Operador entre aspas** (`">"`): começa com aspa, então cai em `make_quots_len` e é classificado como `WORD` (regra 7) 🟢
- **Token expandido para vazio:** continua no array com `lex = VAR`; `clean_tokens` gera `""` (detalhado em `expandir-variaveis`) 🟢

## Dependências
- **Libft:** `ft_strchr`, `ft_strncmp`, `ft_strlen`, `ft_strlcpy`, `ft_strdup`, `ft_strtrim`, `ft_free_arr`
- **iniciar-sessao-e-ler-comando:** garante as aspas balanceadas (`verify_quots`) e chama `facade`
- **expandir-variaveis:** roda entre `sintax_analysis` e `clean_tokens`, usa `lex` e reescreve `tks`
- **executar-comando-simples:** `creat_list_cmd` / `make_args` consomem `lex` e `ctks`

## Decisões de Design Identificadas

| Decisão | Evidência no código | Confiança |
|---------|---------------------|-----------|
| Arrays paralelos `tks`/`lex`/`ctks` indexados pela posição (ADR-003) | `includes/minishell.h:63-67` | 🟢 |
| Lexemas como strings, não `enum` | `src/tokens/lexical_analyzer.c:36-50` | 🟢 |
| Contagem separada do corte, para alocar antes | `src/tokens/count_tks.c`, `src/tokens/tokenizer.c:28` | 🟢 |
| A gramática é verificada **antes** da expansão, sobre os lexemas originais | `src/facade.c:23,36` | 🟢 |
| A remoção de aspas é feita **depois** da expansão, sobre o texto expandido | `src/facade.c:36-37` | 🟢 |
| Módulo desenvolvido com TDD em GoogleTest (ADR-002) | `tests/*.cpp`, `tests/CMakeLists.txt` | 🟢 |
| Não há modelo formal de *quoting*; os casos foram corrigidos um a um | histórico Git (`fix: echo 'oi'$USER'oi'`) | 🟡 |

## Estado Interno
Sem estado próprio. Tudo vive em `t_line` e dura uma linha 🟢:
- `tks`, `lex` são alocados em `building_tokens`; `ctks` em `clean_tokens`
- Liberados por `free_sintax` (erro) ou `free_line` (fim da linha, `src/free.c:31-45`)
- `tks_nbr` é sobrescrito na linha seguinte

## Observabilidade
Única saída: `Syntax ERROR!\n` em stdout, sem indicar a posição nem o token 🟢. Não há logs.

## Riscos e Lacunas
- 🟢 `count_tks` e `tokenizer` são duas implementações das mesmas regras. Se divergirem, `tokenizer`, `lexical_analyzer` e `clean_tokens` escrevem fora do array. Hoje as duas coincidem para entradas com aspas balanceadas (31 testes de `count_tks`)
- 🟢 `lexical_analyzer` e `clean_tokens` não testam o `malloc`; `building_tokens` não trata `tokenizer == NULL`
- 🟢 `clean_tokens` aloca `tks_nbr + 1` posições, mas itera até `tks[i] == NULL`: depende de as duas contagens coincidirem
- 🟡 `count_inside_quot_as_one_word` usa `*(*line + 1)` como condição de parada, diferente de `make_quots_len`; com aspas desbalanceadas os dois divergiriam, mas o REPL impede esse caso
- ✅ L-07 (decidida em 2026-11-22): documentar o as-is. Nota para uma eventual reimplementação corretiva, fora do escopo atual: não está decidido se BUG-11, BUG-13 e BUG-17 devem ser mantidos por fidelidade ou corrigidos
- ✅ L-04 (respondida em 2026-11-22): **não foi deliberado**; é um bug (BUG-12/RN-13), documentado como comportamento atual (L-07)
