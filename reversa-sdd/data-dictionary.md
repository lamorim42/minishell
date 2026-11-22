# Dicionário de Dados — minishell

> Gerado pelo **Arqueólogo** (Reversa) em 2026-11-11
> Fonte: `includes/minishell.h` e as structs locais dos `.c`
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA

O minishell não tem banco de dados. Este dicionário cobre as estruturas em memória, as constantes e as "categorias léxicas", que funcionam como enum (em forma de string).

---

## 1. `t_line` (`struct s_line`): estado da linha em processamento

Uma única instância em toda a execução, alocada na pilha de `main` e acessível pela global `g_minishell.line`. É reaproveitada a cada linha: `free_line` limpa os campos ao final de cada iteração.

| Campo | Tipo | Inicial (`init_line`) | Descrição | Ciclo de vida |
|-------|------|-----------------------|-----------|---------------|
| `str` | `char *` | `NULL` | Linha crua retornada por `readline` | Alocada pelo readline, liberada em `free_line`/`free_sintax` 🟢 |
| `std_fd[2]` | `int[2]` | não inicializado | Cópias (`dup`) de stdin `[IN=0]` e stdout `[OUT=1]` para restaurar depois de cada linha | Criado uma vez em `main_loop`, fechado em `close_std_fd` 🟢 |
| `ctks` | `char **` | `NULL` | Tokens **sem aspas** (*clean tokens*), paralelo a `tks` | `clean_tokens` 🟢 |
| `tks` | `char **` | `NULL` | Tokens crus; depois da expansão, contêm os valores das variáveis | `tokenizer`, alterado por `expand_var` 🟢 |
| `fd[2]` | `int[2]` | só 2 bytes zerados | **Não é usado** no código atual 🟡 | — |
| `lex` | `char **` | `NULL` | Categoria léxica de cada token (ver §5), paralelo a `tks` | `lexical_analyzer` 🟢 |
| `tks_nbr` | `int` | `0` | Quantidade de tokens (`count_tks`). Define o tamanho de `lex` e `ctks` | 🟢 |
| `envp` | `char **` | — (vem de `main`) | Ambiente **original** do processo. Usado no `execve` (não reflete `export`/`unset`) | Não é dono da memória 🟢 |
| `status_code` | `int` | `0` | Status bruto do `waitpid` **ou** código escrito direto pelos builtins (1, 127) e pelo handler de SIGINT (130) | Convertido em `$?` por `update_status_code` 🟢 |
| `count_cmds` | `int` | `0` | Número de nós de comando na lista | `alloc_commands`, zerado em `free_line` 🟢 |
| `pid` | `int *` | `NULL` | Array (`calloc(count_cmds+1)`) com os pids dos filhos, terminado por 0 | 🟢 |
| `pid_index` | `int` | não inicializado | Próxima posição livre em `pid` | Zerado em `alloc_commands` 🟢 |
| `sig` | `int` | `1` | Flag de contexto de sinal. Recebe 0 nas funções de sinal, mas **nunca é lida** 🟡 | — |
| `list_cmds` | `t_pipe_list *` | `NULL` | Cabeça da lista de comandos e operadores | `creat_list_cmd` / `free_list` 🟢 |

---

## 2. `t_pipe_list` (`struct s_pipe_list`): nó da lista de execução

Lista **duplamente encadeada**. Cada nó é um **comando** ou um **operador** (pipe ou redirecionamento).

| Campo | Tipo | Obrigatório | Descrição |
|-------|------|-------------|-----------|
| `args` | `char **` | sim | Comando: `[cmd, arg1, ..., NULL]`. Operador: `[CATEGORIA, alvo, NULL]` (por exemplo `["REDO", "out.txt"]`) ou `["PIPE", NULL]` 🟢 |
| `bin` *(union)* | `char *` | só em comandos | Caminho resolvido do binário, ou o literal `"builtin"`, ou `NULL` (não encontrado) 🟢 |
| `fd[2]` *(union)* | `int[2]` | só em operadores | `REDI`/`REDO`/`REDA`: `fd[0]` = fd do arquivo aberto. `PIPE`/`HERE`: `fd[0]` = leitura, `fd[1]` = escrita 🟢 |
| `next` | `t_pipe_list *` | — | Próximo nó 🟢 |
| `prev` | `t_pipe_list *` | — | Nó anterior 🟢 |

> ⚠️ `bin` e `fd` dividem a mesma memória (union). Em sistemas de 64 bits, `fd[0]` e `fd[1]` correspondem aos 32 bits baixos e altos do ponteiro `bin`. O código distingue o papel do nó por `args[0]`, mas `find_input` lê `fd[0]` de nós de comando (ver RR-07 em `code-analysis.md`) 🟢.

### Discriminador do nó

| `args[0]` | Papel | Campo ativo do union |
|-----------|-------|----------------------|
| `"PIPE"` | Pipe | `fd` |
| `"REDI"` | Entrada `<` | `fd` |
| `"REDO"` | Saída `>` | `fd` |
| `"REDA"` | Append `>>` | `fd` |
| `"HERE"` | Heredoc `<<` | `fd` |
| qualquer outro | Comando | `bin` |

Atenção: um comando chamado literalmente `PIPE`, `REDO`, `REDI`, `REDA`, `HERE` (ou qualquer palavra **que comece** com esses nomes, porque a comparação é `ft_strncmp` com n=4) é tratado como operador 🟢.

---

## 3. `t_hash_table` / `t_hash_item`: ambiente

### `t_hash_table`

| Campo | Tipo | Descrição |
|-------|------|-----------|
| `item` | `t_hash_item **` | Array de buckets (`SIZE_TABLE` = 149) 🟢 |
| `size` | `int` | 149 🟢 |
| `count` | `int` | Número de itens. É mantido, mas nunca lido 🟢 |

### `t_hash_item`

| Campo | Tipo | Descrição |
|-------|------|-----------|
| `key` | `char *` | Nome da variável (cópia própria) 🟢 |
| `value` | `char *` | Valor (cópia própria). `""` para variáveis sem valor no `envp` 🟢 |
| `next` | `t_hash_item *` | Próximo item na lista de colisão 🟢 |

### Chaves especiais

| Chave | Origem | Uso |
|-------|--------|-----|
| `?` | Inserida em `population_hash_table` com `"0"` e atualizada em `update_status_code` | Expandida como `$?`. **Aparece em `env` e `export`** 🟢 |
| `PATH` | `envp` | `path_finder` 🟢 |
| `HOME` | `envp` | `cd` sem argumento ou com `~` 🟢 |
| `PWD` / `OLDPWD` | `envp`, atualizadas por `cd` | `pwd` lê `PWD` 🟢 |

---

## 4. `struct s_minishell`: estado global

| Campo | Tipo | Descrição |
|-------|------|-----------|
| `line` | `t_line *` | Aponta para a `t_line` de `main` 🟢 |
| `table` | `t_hash_table *` | Aponta para a tabela. ⚠️ É atribuída **uma vez** em `main` e não é atualizada se o ponteiro da tabela mudar. Na prática a tabela nunca é realocada 🟢 |

Instância: `g_minishell` (`src/main.c:15`), declarada como `extern` em `signals*.c` e `builtins/*.c` 🟢.

---

## 5. Categorias léxicas (enum em forma de string)

Produzidas por `find_lexema` (`src/tokens/lexical_analyzer.c:33`). A ordem de avaliação importa.

| Valor | Condição (na ordem) | Significado |
|-------|---------------------|-------------|
| `PIPE` | token começa com `\|` | Pipe 🟢 |
| `REDA` | token == `>>` | Redirecionamento de saída em append 🟢 |
| `HERE` | token == `<<` | Heredoc 🟢 |
| `REDO` | token começa com `>` | Redirecionamento de saída (truncar) 🟢 |
| `REDI` | token começa com `<` | Redirecionamento de entrada 🟢 |
| `VAR` | contém `$` e tamanho > 1 | Palavra que precisa de expansão 🟢 |
| `WORD` | qualquer outro caso | Palavra literal 🟢 |

---

## 6. Structs locais

| Struct | Arquivo | Campos | Uso |
|--------|---------|--------|-----|
| `struct s_hd` | `src/exec/here_doc.c:15` | `content`, `temp`, `buffer` (todos `char*`) | Acumulador do texto do heredoc 🟢 |
| `struct s_export` | `src/builtins/export.c:17` | `current`, `temp` (`t_hash_item*`), `i`, `counter` (`int`) | Iteração na impressão do `export` (`counter` não é usado) 🟢 |

---

## 7. Constantes e macros

| Nome | Valor | Arquivo | Uso |
|------|-------|---------|-----|
| `SIZE_TABLE` | `149` | `minishell.h` | Número de buckets e módulo da função hash 🟢 |
| `TRUE` / `FALSE` | `1` / `0` | `minishell.h` | Booleanos 🟢 |
| `IN` / `OUT` | `0` / `1` | `minishell.h` | Índices de `std_fd` 🟢 |
| Prompt | `"miau> "` | `main.c:45` | Prompt principal 🟢 |
| Prompt do heredoc | `"> "` | `here_doc.c:43` | 🟢 |
| Delimitadores de chave de variável | `"',:./[]{}=-+*!@# ` (com espaço) | `expand_var_utils.c:44` | Fim do nome da variável 🟢 |
| Delimitadores de token | `|><` + espaço (`$` também, depois de aspas) | `tokenizer.c` | 🟢 |
| Permissão de arquivo criado | `0644` | `fd_handler_utils.c:45,48` | 🟢 |
| Buffer do `cd` | `500` bytes | `cd.c:22` | Caminho máximo suportado pelo `getcwd` 🟢 |

## 8. Códigos de saída

| Código | Situação |
|--------|----------|
| 0 | Sucesso; Ctrl-D no prompt; builtin executado em um filho (sempre) 🟢 |
| 1 | Arquivo de entrada inexistente (no filho); erros de `cd`, `export`, `unset`; `exit` com argumentos demais 🟢 |
| 2 | `exit` com argumento não numérico 🟢 |
| 127 | Comando não encontrado; falha de `execve`; `env` com argumento; nome que parece builtin mas não é 🟢 |
| 128+N | Filho terminado pelo sinal N (por exemplo, 130 para SIGINT) 🟢 |
| 130 | Ctrl-C no heredoc (status do filho do heredoc) 🟢 |
