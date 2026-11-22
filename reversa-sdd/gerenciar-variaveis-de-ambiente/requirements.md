# Gerenciar Variáveis de Ambiente

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash
> Casos "(observado)" foram executados contra o `main.c` legado compilado na scratchpad (readline substituto lendo stdin). Ambiente de teste: `HOME=/home/u USER=luis PATH=/usr/bin:/bin`. O repositório não foi alterado.

## Visão Geral
Mantém o ambiente do shell em uma tabela hash própria, carregada do `envp` na inicialização, e oferece os builtins `export`, `unset` e `env` para consultá-la e alterá-la. A tabela também guarda `$?` (chave `?`). Ela é lida pela expansão de variáveis, pela busca no `PATH` e pelo `cd` 🟢.

## Responsabilidades
- Armazenar pares chave/valor em uma tabela hash de 149 posições com encadeamento 🟢
- `export K=V`: criar ou atualizar variáveis 🟢
- `export` sem argumentos: listar as variáveis no formato `declare -x K="V"` 🟢
- `unset K`: remover variáveis 🟢
- `env`: listar as variáveis no formato `K=V` 🟢
- Validar identificadores e informar erros 🟢

## Regras de Negócio

### `export`
- EX-01: `export K=V` cria `K` ou atualiza o valor: `export A=1 B=2` cria as duas; `export A=3` atualiza 🟢 (observado)
- EX-02: o identificador é válido quando o **primeiro caractere é letra** (`ft_isalpha`); os outros não são verificados 🟢
  - `_X=1` → `_X=1: not a valid identifier`, `$?=1` 🟢 (observado). ⚠️ O bash aceita `_`
  - `1A=2` → `not a valid identifier`, `$?=1` 🟢 (observado), igual ao bash
  - `A-B=1` é **aceito** e aparece no `env` 🟢 (observado). ⚠️ O bash rejeita
- EX-03: `export K` (sem `=`) é **ignorado** em silêncio 🟢 (observado). ⚠️ O bash marca `K` para exportação e a lista no `export`
- BUG-06: `export K=` (valor vazio) é **ignorado**; `K` não é criada 🟢 (observado). ⚠️ O bash cria `K` vazia
- BUG-06: valor com `=` é corrompido: `export E=b=c` → `E` = `b=b=` 🟢 (observado). ⚠️ O bash grava `b=c`
- EX-04: o valor chega já expandido e sem aspas (`export A=$HOME` → `A=/home/u`) 🟢 (observado em `expandir-variaveis`)
- EX-05: `export` sem argumentos lista **todas** as chaves, **inclusive `?`**, na ordem dos buckets da tabela (não ordenada): `declare -x ?="0"` 🟢 (observado). ⚠️ O bash ordena e não mostra `?`
- EX-06: identificadores inválidos não interrompem o laço; os válidos seguintes são processados 🟢

### `unset`
- UN-01: `unset K` remove `K` se existir; se não existir, não faz nada 🟢
- UN-02: argumento cujo primeiro caractere não é letra → `<arg>: not a valid identifier`, `$?=1`: `unset 9`, `unset ?` 🟢 (observado). Isso protege a chave `?`
- BUG-07: `unset` **falha em silêncio** quando a chave é a **cabeça** de uma lista de colisão: com `A` e `kk` no mesmo bucket (soma dos caracteres ≡ 65 mod 149), `unset A` não remove `A` 🟢 (observado). ⚠️ No bash sempre remove

### `env`
- EN-01: `env` sem argumentos lista `K=V` de todas as chaves da tabela, **inclusive `?=<status>`**, na ordem dos buckets 🟢 (observado). ⚠️ O bash não mostra `?`
- EN-02: `env <qualquer coisa>` → `<arg>: No such file or directory`, `$?=127`; não executa programas 🟢 (observado). ≈ O bash (`env` do coreutils) executaria o programa

### Escopo
- RN-37 / BUG-03: as alterações **não** chegam aos processos filhos, porque o `execve` usa o `envp` original (detalhado em `executar-comando-simples`) 🟢 (observado)
- RE-01: `export`/`unset` só têm efeito quando são o último nó da linha; `export X=1 > f` ou `export X=1 | cat` não alteram nada 🟢 (observado)
- A tabela é carregada do `envp`: `K=V` com `=` dentro do valor preserva o valor (`preper_to_insert`); `K` sem `=` vira `K=""` (unit `iniciar-sessao-e-ler-comando`) 🟢
- ✅ L-03 (respondida em 2026-11-22): é um **bug** (BUG-03), confirmado pelo autor; documentado como comportamento atual (L-07)
- ✅ L-07 (decidida em 2026-11-22, [Revisão]): **documentar o comportamento atual**. As ⚠️ DIVERGÊNCIAS desta unit são o comportamento especificado, não defeitos a corrigir. (Pendência original: BUG-06, BUG-07, EX-02, EX-03, EX-05 e EN-01)

## Requisitos Funcionais

| ID | Requisito | Prioridade | Critério de Aceite |
|----|-----------|-----------|-------------------|
| RF-01 | `export K=V` cria ou atualiza `K` | Must | `export A=1`; `echo $A` → `1` |
| RF-02 | `export` sem argumentos lista no formato `declare -x K="V"` | Should | Saída contém `declare -x HOME="/home/u"` |
| RF-03 | Rejeitar identificadores que não começam com letra, com `$?=1` | Must | `export 1A=2` → mensagem e `$?=1` |
| RF-04 | `unset K` remove `K` | Must | `export A=1`; `unset A`; `echo [$A]` → vazio (ver BUG-05) |
| RF-05 | `unset` com identificador inválido → mensagem e `$?=1` | Should | `unset 9` → `9: not a valid identifier` |
| RF-06 | `env` lista `K=V` | Must | Saída contém `HOME=/home/u` |
| RF-07 | `env` com argumento → `No such file or directory` e `$?=127` | Could | `env x` → mensagem |
| RF-08 | Guardar `?` na tabela com o último status | Must | Ver `expandir-variaveis` RF-03 |

## Requisitos Não Funcionais

| Tipo | Requisito inferido | Evidência no código | Confiança |
|------|--------------------|---------------------|-----------|
| Desempenho | Tabela hash fixa de 149 buckets; função de hash = soma dos bytes mod 149 (muitas colisões em chaves com as mesmas letras) | `includes/minishell.h` (`SIZE_TABLE`), `src/hash_table/hash_function.c` | 🟢 |
| Consistência | Atualização = `table_delete` + `hash_insert`; não é atômica | `src/builtins/cd.c:42-51` | 🟢 |
| Memória | Chave e valor duplicados com `ft_strdup` em cada item; `search_item` devolve cópia | `src/hash_table/create_node.c`, `src/hash_table/search_item.c` | 🟢 |

## Critérios de Aceitação

```gherkin
Funcionalidade: Variáveis de ambiente

  Cenário: Criar e ler
    Quando o usuário digita "export A=1 B=2"
    E depois digita "echo $A $B"
    Então a saída é "1 2"

  Cenário: Atualizar
    Dado A=1
    Quando o usuário digita "export A=3"
    E depois digita "echo $A"
    Então a saída é "3"

  Cenário: Identificador inválido
    Quando o usuário digita "export 1A=2"
    Então "1A=2: not a valid identifier" é escrito em stderr
    E $? é 1

  Cenário: Remover
    Dado A=1
    Quando o usuário digita "unset A"
    Então A não existe mais

  Cenário: env com argumento
    Quando o usuário digita "env x"
    Então "x: No such file or directory" é escrito em stderr
    E $? é 127

  @divergente
  Cenário: Valor com "=" corrompido (BUG-06)
    Quando o usuário digita "export E=b=c"
    E depois digita "echo $E"
    Então a saída é "b=b="

  @divergente
  Cenário: Valor vazio ignorado (BUG-06)
    Quando o usuário digita "export D="
    Então D não é criada

  @divergente
  Cenário: unset em cabeça de colisão (BUG-07)
    Dado "export A=1 kk=2"
    Quando o usuário digita "unset A"
    E depois digita "echo $A"
    Então a saída é "1"

  @divergente
  Cenário: env mostra a chave "?" (EN-01)
    Quando o usuário digita "env"
    Então a saída contém "?=0"
```

## Prioridade (MoSCoW)

| Requisito | MoSCoW | Justificativa |
|-----------|--------|---------------|
| RF-01, RF-03, RF-04, RF-06, RF-08 | Must | `export`, `unset` e `env` são obrigatórios no subject; a tabela alimenta expansão e `PATH` |
| RF-02, RF-05 | Should | Saída e mensagens auxiliares |
| RF-07 | Could | Caso raro |

## Rastreabilidade de Código

| Arquivo | Função | Cobertura |
|---------|--------|-----------|
| `src/builtins/export.c` | `export_builtin`, `building_var`, `print_export`, `print_var_line` | 🟢 |
| `src/builtins/unset.c` | `unset_builtin` | 🟢 |
| `src/builtins/env.c` | `env_builtin` | 🟢 |
| `src/builtins/builtins_utils.c` | `is_args_ok`, `insert_var_in_hash`, `insert_simple_var_in_hash` | 🟢 |
| `src/builtins/cd.c` | `update_var_env` | 🟢 |
| `src/hash_table/create_table.c` | `create_table`, `population_hash_table`, `count_split_chr`, `preper_to_insert` | 🟢 |
| `src/hash_table/insert_node.c` | `hash_insert`, `handle_collision` | 🟢 |
| `src/hash_table/delete_node.c` | `table_delete`, `delete_node` | 🟢 |
| `src/hash_table/search_item.c` | `search_item` | 🟢 |
| `src/hash_table/print_table.c` | `print_table` | 🟢 |
| `src/hash_table/hash_function.c` | `hash_function` | 🟢 |
| `src/hash_table/create_node.c`, `free_hash_table.c` | `create_item`, `free_item`, `free_table` | 🟢 |
