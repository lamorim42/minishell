# Análise de Código — minishell (MiauShell)

> Gerado pelo **Arqueólogo** (Reversa) em 2026-11-11 · nível **completo**
> Escala: 🟢 CONFIRMADO (lido no código ou observado em execução) · 🟡 INFERIDO · 🔴 LACUNA
>
> **Como os comportamentos foram verificados:** além da leitura do código, o Arqueólogo compilou uma cópia isolada (`git archive HEAD`) na scratchpad da sessão, com um stub de readline baseado em `getline`, e rodou o binário com scripts pelo stdin. Os itens marcados como "observado" vêm dessas execuções. O repositório não foi alterado.

---

## 0. Visão do pipeline (pipe-and-filter)

Cada linha digitada passa por uma sequência fixa de "filtros". O estado da linha vai de um filtro a outro dentro de `t_line`.

| # | Filtro | Função | Arquivo | Saída em `t_line` |
|---|--------|--------|---------|-------------------|
| 1 | Leitura | `readline("miau> ")` | `src/main.c:45` | `str` |
| 2 | EOF (Ctrl-D) | `control_d` | `src/main.c:65` | encerra o processo |
| 3 | Aspas balanceadas | `verify_quots` | `src/tokens/sintax_analysis.c:46` | — |
| 4 | Comentário | `is_a_comment` | `src/main.c:83` | — |
| 5 | Contagem de tokens | `count_tks` | `src/tokens/count_tks.c:21` | `tks_nbr` |
| 6 | Tokenização | `tokenizer` | `src/tokens/tokenizer.c:20` | `tks` |
| 7 | Classificação léxica | `lexical_analyzer` | `src/tokens/lexical_analyzer.c:17` | `lex` |
| 8 | Sintaxe | `sintax_analysis` | `src/tokens/sintax_analysis.c:17` | bool |
| 9 | Expansão de `$VAR` | `expand_var` | `src/exec/expand_var.c:19` | `tks` (alterado no lugar) |
| 10 | Remoção de aspas | `clean_tokens` | `src/tokens/clean_tokens.c:18` | `ctks` |
| 11 | Lista de comandos | `creat_list_cmd` | `src/exec/creat_cmd_table.c:18` | `list_cmds` |
| 12 | Pipes de heredoc | `creat_here_doc` | `src/exec/here_doc_utils.c:42` | `node->fd` |
| 13 | Leitura do heredoc (processo filho) | `here_doc_verification` | `src/exec/here_doc_utils.c:15` | conteúdo escrito no pipe |
| 14 | Resolução do binário | `list_generation_bin` | `src/exec/list_generation_bin.c:17` | `node->bin` |
| 15 | Contagem de comandos | `alloc_commands` | `src/facade.c:61` | `count_cmds`, `pid[]` |
| 16 | Abertura de fds | `open_fds` | `src/fd_handler_utils.c:17` | `node->fd` |
| 17 | Execução | `exec_list` | `src/exec/exec_list.c:18` | processos, `$?` |
| 18 | Histórico e limpeza | `add_history`, `free_line` | `src/facade.c:57-58` | — |

🟢 Toda a sequência foi extraída de `src/main.c` e `src/facade.c`.

---

## 1. Módulo `core`

**Arquivos:** `src/main.c`, `src/facade.c`, `src/init.c`, `src/free.c`, `src/arr_utils.c`
**Propósito:** loop interativo (REPL), orquestração por linha (Facade), inicialização, liberação de recursos e utilitários de array.

### 1.1 Funções

| Função | Assinatura | Responsabilidade |
|--------|-----------|------------------|
| `main` | `int main(int argc, char **argv, char **envp)` | Inicializa `t_line`, copia `envp` para a hash table, preenche a global `g_minishell` e entra no loop 🟢 |
| `main_loop` (static) | `void (t_line*, t_hash_table**)` | Faz `dup` de stdin/stdout em `std_fd`. Loop infinito: sinais → readline → EOF → aspas → comentário → `facade` → restaura stdin/stdout com `dup2` 🟢 |
| `control_d` (static) | `void (char*, t_hash_table**)` | Se `readline` retorna `NULL`: libera a tabela, escreve `exit\n` em **stderr** e chama `exit(0)` 🟢 |
| `is_a_comment` (static) | `int (char*)` | Pula **apenas espaços** (não tabs) e retorna TRUE se o primeiro caractere for `#` 🟢 |
| `file_exists` | `int (int fd)` | `fstat(fd) == 0`. É usado como "fd válido?" 🟢 |
| `facade` | `void (t_line*, t_hash_table**)` | Se a linha não está vazia: tokeniza e checa a sintaxe. Em caso de erro chama `free_sintax`; senão chama `exec_pipe_line` 🟢 |
| `exec_pipe_line` (static) | `void (t_line*, t_hash_table**)` | Roda os filtros 9 a 18. **Sempre faz um fork** para o heredoc, mesmo sem `<<` na linha 🟢 |
| `alloc_commands` (static) | `void (t_line*)` | Conta os nós que são comando (`is_command`) e aloca `pid[count+1]` com `ft_calloc` 🟢 |
| `init_line` | `void (t_line*)` | Zera os campos. `sig = 1`. `ft_bzero(line->fd, 2)` zera só 2 **bytes** de um `int[2]` 🟢 |
| `free_line` | `void (t_line*)` | Libera lista, `tks`, `lex`, `pid`, `ctks` e `str`, e zera `count_cmds`. **Não** atribui `NULL` a `line->str` 🟢 |
| `free_sintax` | `void (t_line*)` | Imprime `Syntax ERROR!` em **stdout**, libera `tks`, `lex` e `str` 🟢 |
| `free_list` | `void (t_pipe_list*)` | Libera os nós. Só libera `bin` em nós cujo `args[0]` não é token de operador 🟢 |
| `error_msg` | `void (char *str, char *msg)` | Escreve `str + msg` em stderr 🟢 |
| `free_exit` | `void (t_line*, t_hash_table*, int)` | Libera tudo e chama `exit(status)`. Usada nos processos filhos 🟢 |
| `copy_array` | `char **(char **tokens, int size)` | Duplica os `size` primeiros elementos. Aloca `ft_array_len(tokens)+1` 🟢 |
| `ft_array_len` | `int (char**)` | Conta elementos até `NULL` 🟢 |
| `print_array` | `void (char*, char**)` | Debug com `dprintf(2, ...)` 🟢 |

### 1.2 Regras observadas

- **RC-01** Linha com aspas desbalanceadas → `Syntax ERROR!` em stdout, sem execução, `$?` inalterado 🟢 (observado).
- **RC-02** Linha cujo primeiro caractere depois dos espaços é `#` é ignorada. `#` no meio da linha é texto comum 🟢.
- **RC-03** A checagem de aspas vem **antes** da checagem de comentário: `# it's` dá erro de sintaxe 🟢.
- **RC-04** Linha vazia (`""`) não é processada. Linha só com espaços passa pelo pipeline inteiro, inclusive o fork do heredoc 🟢.
- **RC-05** Só entram no histórico as linhas que chegam à execução (`add_history` fica dentro de `exec_pipe_line`). Linhas com erro de sintaxe, vazias ou comentários **não** entram 🟢.
- **RC-06** Ctrl-D no prompt escreve `exit\n` + um byte `\0` em stderr (`write(2, "exit\n", 6)`) e sai com código 0 🟢 (observado).
- **RC-07** Argumentos de linha de comando do `./minishell` são ignorados. A guarda `argv[0] == NULL && argc > 1` nunca é verdadeira na prática 🟡.
- **RC-08** stdin e stdout originais são restaurados com `dup2` no fim de cada iteração 🟢.

---

## 2. Módulo `tokens`

**Arquivos:** `src/tokens/count_tks.c`, `tokenizer.c`, `tokenizer_utils.c`, `lexical_analyzer.c`, `sintax_analysis.c`, `clean_tokens.c`
**Propósito:** transformar a string crua em tokens, classificá-los, validar a gramática e remover as aspas. É o único módulo com testes (GTest).

### 2.1 Funções

| Função | Assinatura | Responsabilidade |
|--------|-----------|------------------|
| `building_tokens` | `void (t_line*)` | `tks_nbr = count_tks`; `tks = tokenizer`; `lex = lexical_analyzer` 🟢 |
| `count_tks` | `int (char *line)` | Conta tokens com as mesmas regras do tokenizer. Esse número define o tamanho dos arrays 🟢 |
| `tokenizer` | `char **(t_line*)` | Percorre `str`, ignora espaços e chama `worddup` para cada token 🟢 |
| `worddup` (static) | `char *(char **s)` | Escolhe a regra de tamanho pelo primeiro caractere, copia e avança o ponteiro 🟢 |
| `make_red_len` (static) | `void (char*, int*)` | `|`, `<`, `>` → 1 caractere. `>>` e `<<` → 2 🟢 |
| `make_quots_len` (static) | `void (char*, int*)` | Consome até a aspa de fechamento e continua até achar `|><$ `. Se encontrar outra aspa, consome recursivamente (`"a"'b'c` vira um token só) 🟢 |
| `make_word_len` (static) | `void (char*, int*)` | Consome até `|>< `. Se encontrar aspa: quando a palavra **não** começa com `$`, passa para `make_quots_len` (fica no mesmo token); quando começa com `$`, **corta** o token ali 🟢 |
| `lexical_analyzer` | `char **(t_line*)` | Gera um array paralelo `lex[]` de categorias 🟢 |
| `find_lexema` (static) | — | `|`→`PIPE`, `>>`→`REDA`, `<<`→`HERE`, `>`→`REDO`, `<`→`REDI`, contém `$` e tamanho > 1 → `VAR`, senão `WORD` 🟢 |
| `sintax_analysis` | `int (char **lex)` | Valida a gramática (regras abaixo) 🟢 |
| `verify_quots` | `int (char*)` | Toda aspa aberta precisa ser fechada pela mesma aspa. Aspas de um tipo dentro de outro são literais 🟢 |
| `clean_tokens` | `char **(t_line*)` | Gera `ctks[]` com as aspas removidas 🟢 |
| `clean_quots` (static) | — | Se o token começa **e** termina com `"`: `ft_strtrim(tks, "\"")`. Igual para `'`. Senão: `clean_in_middle` 🟢 |
| `clean_in_middle` (static) | — | Remove **todas** as aspas dos dois tipos 🟢 |
| `count_quots` | `int (char*)` | Conta `"` e `'` 🟢 |
| `ft_strcmp_len` | `char (char*, char*)` | Igualdade exata de strings: mesmo tamanho e `strncmp == 0` 🟢 |

### 2.2 Algoritmo de tokenização

Regras de corte, extraídas de `tokenizer.c` e `count_tks.c`:

1. Espaço (só `' '`, sem tab) separa tokens.
2. `|`, `<`, `>` são tokens próprios. `<<` e `>>` são tokens de 2 caracteres. `<<<` vira `<<` + `<`, e `<>` vira `<` + `>`.
3. Um token que começa com aspa vai até a aspa de fechamento e continua até um delimitador `|><$ `. Isso significa que **`$` dentro de uma palavra que veio depois de aspas corta o token**: `"a"$X` gera `"a"` e `$X`.
4. Palavras que começam com `$` cortam o token na primeira aspa: `$X'q'` vira `$X` e `'q'`. Como cada token vira um argumento, `echo $X'q'` imprime `hello q` em vez de `helloq` 🟢 (observado).
5. Palavras que não começam com `$` absorvem aspas: `a"b c"d` é um token só.

### 2.3 Gramática (`sintax_analysis`)

| Regra | Rejeita |
|-------|---------|
| G-01 | `PIPE` na primeira posição |
| G-02 | `PIPE` seguido de `PIPE` |
| G-03 | `REDO`, `REDA`, `REDI` ou `HERE` não seguidos **exatamente** de `WORD` |

Consequências:
- **G-03 rejeita `VAR` como alvo de redirecionamento.** `> $FILE`, `> "$x"`, `<< $EOF` e `> a$b` dão erro de sintaxe 🟢.
- **`PIPE` no fim da linha é aceito.** `ls |` executa `ls` com a saída indo para um pipe que ninguém lê 🟢 (observado: nenhuma mensagem).
- Erro de sintaxe → `Syntax ERROR!` em stdout. `$?` **não** vai para 2 🟢.

### 2.4 Remoção de aspas (divergências em relação ao bash)

| Entrada | minishell | bash |
|---------|-----------|------|
| `"a"b"c"` | `a"b"c` 🟢 (observado) | `abc` |
| `a"b'c"` | `abc` (remove todas as aspas) 🟢 | `ab'c` |
| `"it's"` | `it's` 🟢 | `it's` |
| Aspas vindas da expansão de variável | removidas (a limpeza roda **depois** da expansão) 🟢 | preservadas |

---

## 3. Módulo `exec`

**Arquivos:** `src/exec/*.c` (13 arquivos)
**Propósito:** expansão de variáveis, montagem da lista de comandos, heredoc, resolução de binários, fork/execve, espera e `$?`.

### 3.1 Funções

| Função | Arquivo | Responsabilidade |
|--------|---------|------------------|
| `expand_var` | `expand_var.c:19` | Para cada token `VAR` cujo **primeiro caractere** não é `'`: repete `change_value` enquanto houver `$` no token 🟢 |
| `change_value` (static) | `expand_var.c:33` | Extrai a chave e busca na tabela. Achou: substitui `$KEY` pelo valor. **Não achou: o token inteiro vira `""`** 🟢 |
| `join_str_value` (static) | `expand_var.c:57` | Monta prefixo + valor + sufixo 🟢 |
| `return_var_key` / `key_len` | `expand_var_utils.c` | A chave começa depois de `$` e vai até um delimitador do conjunto `"',:./[]{}=-+*!@# ` 🟢 |
| `take_prefix_var` / `sufix_var` | `exec_utils.c:24,38` | Substrings antes e depois de `$KEY` 🟢 |
| `is_command` | `exec_utils.c:15` | `args[0]` não é `PIPE`, `REDO`, `REDA`, `REDI` nem `HERE` 🟢 |
| `creat_list_cmd` | `creat_cmd_table.c:18` | Agrupa os tokens em nós da `t_pipe_list` 🟢 |
| `make_args` | `creat_cmd_table.c:27` | Copia `size` tokens limpos. Em nós de operador, troca `args[0]` pela categoria léxica (`"REDO"` etc.) 🟢 |
| `get_arg_size` | `creat_cmd_table.c:44` | Sequência de `WORD`/`VAR` → n. `PIPE` → 1. Redirecionamento → 2 (operador + alvo) 🟢 |
| `char_cat` | `creat_cmd_table.c:69` | `str + c`, em memória nova 🟢 |
| `add_back_list` / `new_node` | `make_list.c` | Lista duplamente encadeada 🟢 |
| `creat_here_doc` | `here_doc_utils.c:42` | `pipe()` para cada nó `HERE` 🟢 |
| `here_doc_verification` | `here_doc_utils.c:15` | No processo filho: para cada `HERE`, lê o conteúdo e escreve no pipe 🟢 |
| `here_doc_buffer` / `generation_content` | `here_doc.c` | Loop `readline("> ")` até uma linha **igual** ao delimitador (`args[1]`). Com EOF imprime `<delim>: Wanted` 🟢 |
| `here_doc_write` | `here_doc_utils.c:34` | Escreve o buffer e fecha as duas pontas no filho 🟢 |
| `close_here_doc` | `here_doc_utils.c:56` | No pai: fecha a ponta de escrita 🟢 |
| `list_generation_bin` | `list_generation_bin.c:17` | Builtin → `bin = "builtin"`. Caminho (`/`, `./`, `../`) → literal. Senão `path_finder` 🟢 |
| `path_finder` | `path_finder.c:65` | Busca `PATH` na **hash table**, faz `split(':')` e testa `access(dir/cmd, F_OK)`. Retorna o primeiro que **existe** (não checa permissão de execução) 🟢 |
| `exec_list` | `exec_list.c:18` | Percorre os nós de comando. Builtin **no último nó da lista** roda no **pai**; o resto passa por `init_fork` 🟢 |
| `wait_child` (static) | `exec_list.c:42` | `waitpid` em cada pid, na ordem. `status_code` fica com o status do **último** 🟢 |
| `update_status_code` (static) | `exec_list.c:54` | Converte o status em `$?` (`WEXITSTATUS`, ou `128 + sinal`) e grava em `"?"` na tabela 🟢 |
| `init_fork` | `init_fork.c:17` | Faz fork do comando e guarda o pid. Fecha o pipe anterior no pai 🟢 |
| `exec_path` | `exec_path.c:17` | No filho: redirecionamentos → `bin == NULL` (127, `command not found`) → builtin (`exit 0`) → `execve` (em erro, `perror` e 127) 🟢 |
| `make_redirections` (static) | `exec_path.c:41` | `find_input`, `find_output`, `close_fds` 🟢 |

### 3.2 Algoritmo de montagem da lista

Os tokens são consumidos em blocos:

```
echo a b > out | wc -l < in
→ [echo a b] [REDO out] [PIPE] [wc -l] [REDI in]
```

- Um nó de **operador** guarda em `args[0]` a categoria (`"REDO"`) e em `args[1]` o alvo 🟢.
- **Argumentos depois de um redirecionamento formam um novo nó de comando.** `cat < f -e` vira `[cat] [REDI f] [-e]`, e `-e` é executado como um segundo comando 🟢 (por leitura do código).
- O union `bin` / `fd[2]` faz com que nós de comando usem `bin` e nós de operador usem `fd` na mesma memória 🟢.

### 3.3 Regras de execução

- **RE-01** Builtin roda no pai só quando é o **último nó** da lista (`temp->next == NULL`). Isso tem três efeitos:
  - `ls | exit 3` **encerra o shell** com código 3 🟢 (observado). O mesmo vale para `ls | cd /tmp`.
  - `cd /tmp > /dev/null` roda em um filho e **não muda** o diretório 🟢 (observado).
  - `export X=1 > f` não tem efeito 🟢.
- **RE-02** Builtin executado em um filho sempre sai com **0** (`free_exit(..., 0)`): `cd nope > /dev/null; echo $?` → `0` 🟢 (observado).
- **RE-03** O `execve` recebe o **`envp` original** de `main`, não a hash table. `export` e `unset` **não afetam** o ambiente dos processos filhos: `export ZZ=1; sh -c 'echo $ZZ'` imprime vazio 🟢 (observado).
- **RE-04** Comando não encontrado → `cmd: command not found` em stderr, código 127 🟢.
- **RE-05** Falha do `execve` (por exemplo, um diretório) → `perror` e **127**. O bash usaria 126 🟢 (observado: `/etc: Permission denied`, `$?=127`).
- **RE-06** A busca no `PATH` usa `F_OK`: um arquivo sem permissão de execução é escolhido 🟢.
- **RE-07** Comando relativo sem `./` (por exemplo `bin/foo`) é procurado só no `PATH` 🟢.
- **RE-08** O heredoc é lido **sempre** em um processo filho separado, antes de qualquer comando 🟢. O pai não confere o status desse filho. Se o usuário der Ctrl-C no heredoc, o filho sai com 130 e **o pai continua** e executa os comandos mesmo assim 🟡 (comportamento real com terminal ainda não observado).
- **RE-09** O conteúdo do heredoc fica em um `pipe()` que só é lido depois. Conteúdo maior que a capacidade do pipe (~64 KiB no Linux) **trava** o filho 🟡.
- **RE-10** O heredoc **não** expande variáveis. O delimitador é comparado já sem aspas 🟢.

### 3.4 Regras de expansão

- **RX-01** Só são expandidos tokens classificados como `VAR` (contêm `$` e têm tamanho > 1) cujo **primeiro caractere** não é `'`. `a'$HOME'` **é** expandido 🟢.
- **RX-02** Variável inexistente → **o token inteiro** vira `""`: `abc$NOPE` → `` 🟢 (observado).
- **RX-03** `$A$B` → a chave vira `A$B` (o `$` não é delimitador) → normalmente não existe → token vazio. `echo $HOME$USER` imprime uma linha vazia 🟢 (observado).
- **RX-04** `"$"` → chave vazia → token vazio. `echo $` (token de tamanho 1, `WORD`) imprime `$` 🟢.
- **RX-05** `$?` funciona porque `?` não é delimitador e a chave `"?"` existe na tabela 🟢.
- **RX-06** Se o valor expandido contém `$`, o loop `while (ft_strchr(tks, '$'))` expande de novo. Um valor que se refere a si mesmo pode causar **loop infinito** 🟡.
- **RX-07** Não há divisão de palavras (*word splitting*) depois da expansão: `X="a b"; ls $X` passa um argumento só 🟡.

---

## 4. Módulo `redirections`

**Arquivos:** `src/fd_handler.c`, `src/fd_handler_utils.c`
**Propósito:** abrir os arquivos de redirecionamento, criar pipes e fazer `dup2` no filho.

### 4.1 Funções

| Função | Responsabilidade |
|--------|------------------|
| `open_fds` | No **pai**, antes de qualquer fork: `REDI` → `open(O_RDONLY)`, `REDO` → `O_RDWR|O_CREAT|O_TRUNC 0644`, `REDA` → `...|O_APPEND 0644`, `PIPE` → `pipe()`. O fd fica em `node->fd[0]` (ou nas duas pontas, no caso do pipe) 🟢 |
| `check_valid_fd` (static) | Erro no open de saída → `perror(alvo)` e **`free_line(line)`** no meio do loop 🟢 |
| `find_input` | Olha **só o nó imediatamente anterior**: se for `PIPE`, `REDI` ou `HERE`, faz `dup2(prev->fd[0], 0)`. Depois percorre os nós **seguintes** até o próximo `PIPE`, guarda o último `REDI`/`HERE` e valida com `file_exists` cada nó visitado 🟢 |
| `find_output` | Percorre os nós **seguintes** até `PIPE`. O último `REDO`/`REDA` vence. Se não houver nenhum e existir `PIPE`, usa `pipe.fd[1]` 🟢 |
| `close_fds` | Volta ao início da lista e fecha os fds de arquivo, de pipe e de heredoc 🟢 |
| `is_redirect_output` / `is_redirect_input` | `REDO`/`REDA` e `REDI`/`HERE` 🟢 |
| `close_std_fd` | Fecha as cópias de stdin/stdout guardadas 🟢 |

### 4.2 Regras e anomalias

- **RR-01** Todos os arquivos de saída são criados ou truncados **no pai, antes da execução**, mesmo os de comandos que depois falham 🟢.
- **RR-02** **Redirecionamento de saída antes do comando é ignorado.** `> f3 ls f1` imprime no terminal e `f3` fica vazio 🟢 (observado), porque `find_output` só olha para frente.
- **RR-03** Redirecionamento de entrada antes do comando só funciona se estiver **imediatamente** antes dele 🟢.
- **RR-04** `ls | > out wc`: o nó anterior a `wc` é `REDO`, então `wc` **não** lê do pipe e a saída **não** vai para `out` 🟢 (observado em um caso equivalente).
- **RR-05** `< inexistente cmd`: `open` retorna -1 e `dup2(-1, 0)` falha em silêncio. O comando roda lendo o stdin original e não aparece mensagem de erro 🟢 (observado). Já `cmd < inexistente` mostra `inexistente: No such file or directory` e sai com 1 🟢 (observado).
- **RR-06** 🔴 **Crash:** erro ao abrir um arquivo de saída (`echo hi > /nonexistent/x`) chama `free_line` dentro de `open_fds`. O loop continua usando nós já liberados (*use-after-free*) e depois `free_line` libera `line->str` de novo (*double free*). O processo **abortou com core dump** 🟢 (observado).
- **RR-07** `find_input` chama `file_exists(temp->fd[0])` também em nós de **comando** que vêm depois (por exemplo `-e` em `cat < f -e`). Nesses nós, `fd[0]` é na verdade parte do ponteiro `bin` (union). O resultado é indefinido 🟡.
- **RR-08** Os fds de `REDI` não são fechados no pai depois da execução. Pode haver vazamento de fd por linha 🟡.

---

## 5. Módulo `builtins`

**Arquivos:** `src/builtins/*.c`
**Propósito:** os comandos internos `echo`, `cd`, `pwd`, `export`, `unset`, `env` e `exit`.

### 5.1 Despacho

- `is_a_builtin` compara com **`ft_strncmp` por prefixo** (`"env"` com n=3 etc.). Retorna **FALSE (0) quando É builtin**, a lógica é invertida 🟢.
- `exec_builtins` compara por **igualdade exata**. Um nome que só tem o prefixo cai no `else`: `command not found` e `status_code = 127` 🟢.
- **Consequência:** comandos externos cujo nome começa com o nome de um builtin **não podem ser executados**: `pwdx`, `envsubst`, `cdrecord`, `exitcode`, `export-foo` etc. → `command not found` 🟢 (observado com `pwdx` e `envsubst`).

### 5.2 Comportamento por builtin

| Builtin | Comportamento extraído | Confiança |
|---------|------------------------|-----------|
| `echo` | Aceita vários `-n` seguidos, mas só no formato exato `-n` (não `-nn`). O `\n` final é omitido se `args[1] == "-n"`. **Argumentos vazios são pulados** (`echo a "" b` → `a b`). Sem argumentos → `\n` | 🟢 (observado) |
| `cd` | Sem argumento ou `~` exato → `$HOME`. Erro → `perror` + status 1. Atualiza `PWD` (com `getcwd`) e `OLDPWD`. Indo para HOME, `PWD` recebe o **valor de HOME**, não o `getcwd`. Buffer fixo de 500 bytes. `cd -` não existe. Argumentos extras são ignorados. `HOME` não definido → `cd: HOME not set`, **status continua 0** | 🟢 (observado) |
| `pwd` | Imprime o valor de `PWD` **da hash table**, não `getcwd`. Sem `PWD` não imprime nada. Ignora argumentos | 🟢 |
| `export` | Sem argumentos → lista `declare -x KEY="VALUE"` na ordem da hash, **incluindo `?`**. Com argumentos: o identificador só é válido se o **primeiro caractere** for letra (`_X` é rejeitado; `A-B=1` é aceito). Só insere se houver `=`. `export A` e `export A=` **não fazem nada** | 🟢 (observado) |
| `export A=b=c` | O valor fica **`b=b=`** por causa de um bug em `insert_var_in_hash`/`building_var` (junta `split[1]` com ele mesmo) | 🟢 (observado) |
| `unset` | Primeiro caractere alfabético → remove. Senão `not a valid identifier` + status 1. **Falha em silêncio** quando a chave é a cabeça de uma lista de colisão (ver hash_table) | 🟢 (observado) |
| `env` | Com argumento → `<arg>: No such file or directory`, status 127. Sem argumento → `KEY=VALUE` de toda a tabela, **incluindo `?=<status>`** | 🟢 (observado) |
| `exit` | Sem argumento → `exit(0)`. Um argumento numérico (`+`/`-` aceitos) → `exit(atoi)`. Não numérico → `<arg> is not a number!` em **stdout** + `exit(2)`. Mais de um argumento (sendo o primeiro numérico) → `exit: too many arguments`, status 1, **não sai**. Não imprime `exit` | 🟢 (observado) |

### 5.3 Códigos de status dos builtins

Os builtins escrevem direto em `g_minishell.line->status_code` (1 ou 127). `update_status_code` trata o valor bruto `1` em um ramo especial (`else if (status_code == 1)`). Quando o builtin roda no pai e dá certo, `$?` herda o status do fork do heredoc (normalmente 0) 🟢.

---

## 6. Módulo `hash_table`

**Arquivos:** `src/hash_table/*.c`
**Propósito:** guardar as variáveis de ambiente e o `$?`.

### 6.1 Algoritmo

- **Função hash:** soma dos códigos ASCII dos caracteres da chave `% 149` (`SIZE_TABLE`). Anagramas sempre colidem (`PATH` e `TAPH` → bucket 3) 🟢.
- **Colisão:** lista encadeada simples por bucket, com inserção no **fim** (`handle_collision`) 🟢.
- **Busca (`search_item`):** percorre a lista e compara com `strncmp(key, len+1)`. Retorna uma **cópia** (`ft_strdup`) que quem chamou precisa liberar 🟢.
- **Inserção (`hash_insert`):** se o bucket está vazio, coloca o item. Se a **cabeça** tem a mesma chave, atualiza o valor. Senão, **adiciona no fim sem procurar duplicata no resto da lista** 🟢.
- **Remoção (`table_delete`):**
  - Bucket com **um só item** → remove **sem conferir se a chave bate** 🟢.
  - Bucket vazio → dereferencia `NULL` (crash) 🟡. Os chamadores verificam com `search_item` antes, exceto `update_status_code`, que apaga `"?"` sem verificar (mas `"?"` sempre existe).
  - Chave na **cabeça** de uma lista com 2 ou mais itens → `delete_node` não remove nada (`temp == NULL`) 🟢 (observado: `export TAPH=1; unset PATH; ls` continua achando o `ls`).
- **Carga inicial (`population_hash_table`):** `split(envp[i], '=')` e reconstrução de valores que contêm `=`. Entrada sem valor → `""`. No fim insere `"?" = "0"` 🟢.
- **`count`** é mantido, mas não é usado em nenhuma decisão 🟢.

---

## 7. Módulo `signals`

**Arquivos:** `src/signals.c`, `src/signals_utils.c`
**Propósito:** tratar SIGINT e SIGQUIT em três contextos.

| Contexto | Instalado por | SIGINT | SIGQUIT |
|----------|---------------|--------|---------|
| Prompt (`readline`) | `signals()` no começo de cada iteração | `sig_handler`: `\n` em stderr, `rl_on_new_line`, `rl_replace_line("")`, `rl_redisplay`. **Não** grava 130 em `$?` | `SIG_IGN` |
| Durante o fork do heredoc (pai e filho) | `signal_ignore()` | `SIG_IGN` | `SIG_IGN` |
| Filho do heredoc | `signal_here()` | `sig_here`: `\n`, libera tudo, `exit(130)` | `SIG_IGN` (herdado) |
| Pai durante a execução | `signals_parent()` (antes de cada fork de comando) | `sig_parent`: `status_code = 130`, `\n`, `rl_on_new_line` | `sig_write`: `Quit\n` em stderr |
| Filho depois do `execve` | — | padrão (o `execve` reseta os handlers) | padrão |
| Filho de builtin (sem `execve`) | herda os do pai | `sig_parent` | `sig_write` |

- A variável global `g_minishell` (`{t_line*, t_hash_table*}`) existe para que `sig_here` e os builtins acessem o estado 🟢.
- Os handlers chamam funções que não são *async-signal-safe* (`printf`, `free`, funções do readline) 🟢. É um risco teórico 🟡.

---

## 8. Módulo `libft`

**Arquivos:** `Libft/*.c`, `Libft/libft.h`
**Propósito:** biblioteca padrão da 42 (reimplementação de parte da libc) mais 3 extras.

- **Mais usadas pelo minishell:** `ft_strncmp` (64 chamadas), `ft_strchr` (24), `ft_strdup` (23), `ft_strlen` (19), `ft_strjoin` (9), `ft_free_arr` (9) 🟢.
- **Extras:**
  - `ft_free_arr`: libera um `char**` terminado em NULL. **Não aceita `NULL`**, porque acessa `mtx[0]` antes de checar 🟢.
  - `ft_arrcmp`: compara arrays de strings (0 = iguais). É usado só nos testes 🟢.
  - `ft_strcmp`: retorna **1 quando são iguais** (semântica invertida em relação à libc). Não é usado em `src/` 🟢.
- `ft_split` descarta campos vazios (`"a==b"` com `=` vira `[a, b]`). Isso afeta `export` e a carga do ambiente 🟢.
- As funções `ft_lst*` não entram no `libft.a` padrão 🟢.

---

## 9. Problemas e anomalias consolidados

| ID | Severidade | Descrição | Local | Confiança |
|----|-----------|-----------|-------|-----------|
| BUG-01 | 🔴 Crash | Falha ao abrir arquivo de saída causa use-after-free e double free | `fd_handler_utils.c:51-55` | 🟢 observado |
| BUG-02 | Alta | Builtin no último nó de um pipe roda no pai (`ls \| exit` encerra o shell) | `exec_list.c:30-32` | 🟢 observado |
| BUG-03 | Alta | `export`/`unset` não chegam aos filhos (o `execve` usa o `envp` original) | `exec_path.c:34` | 🟢 observado |
| BUG-04 | Média | Comandos externos com prefixo de builtin (`pwdx`, `envsubst`) não executam | `builtins_utils.c:40-51` | 🟢 observado |
| BUG-05 | Média | Variável inexistente apaga o token inteiro; `$A$B` não expande | `expand_var.c:48-53`, `expand_var_utils.c:44` | 🟢 observado |
| BUG-06 | Média | `export A=b=c` grava `b=b=`; `export A=` é ignorado | `builtins_utils.c:70-80` | 🟢 observado |
| BUG-07 | Média | `unset` falha quando a chave é a cabeça de uma lista de colisão; `table_delete` apaga sem conferir a chave | `delete_node.c:24-31,47` | 🟢 observado |
| BUG-08 | Média | Redirecionamento de saída antes do comando é ignorado | `fd_handler.c:44-70` | 🟢 observado |
| BUG-09 | Média | `< inexistente cmd` não dá erro | `fd_handler.c:21-24` | 🟢 observado |
| BUG-10 | Média | Builtin em um filho sempre sai com 0 | `exec_path.c:31-32` | 🟢 observado |
| BUG-11 | Média | Remoção de aspas errada em `"a"b"c"` e com aspas mistas | `clean_tokens.c:42-47` | 🟢 observado |
| BUG-12 | Baixa | Erros de sintaxe não gravam `$?=2`; `cd` sem HOME não grava 1 | `free.c:47`, `cd.c:70` | 🟢 |
| BUG-13 | Baixa | `>`/`<`/`<<` seguido de `$VAR` é erro de sintaxe | `sintax_analysis.c:29-40` | 🟢 |
| BUG-14 | Baixa | stdio com buffer herdado pelos forks: a saída do `printf` dos builtins **duplica** quando stdout não é um terminal | `facade.c:40` + `echo.c` | 🟢 observado com stdout em pipe; 🟡 em terminal |
| BUG-15 | Baixa | Ctrl-C no heredoc não cancela a linha | `facade.c:51-56` | 🟡 |
| BUG-16 | Baixa | Falha do `execve` retorna 127 em vez de 126 | `exec_path.c:37` | 🟢 |
| BUG-17 | Baixa | `$X'q'` vira dois argumentos | `tokenizer.c:107-110` | 🟢 observado |
| BUG-18 | Baixa | `init_line` zera só 2 bytes de `fd[2]` | `init.c:27` | 🟢 |
| BUG-19 | Baixa | Ctrl-D escreve um byte `\0` extra | `main.c:70` | 🟢 observado |
| BUG-20 | Média | [Revisão 2026-11-22] `ft_strncmp` da Libft retorna 0 quando uma das strings termina (semântica de prefixo): comando vazio ou prefixo de `PIPE`/`REDO`/`REDA`/`REDI`/`HERE` (`P`, `R x`, `$NOPE ls`) é ignorado em silêncio; é também a causa do BUG-04 | `Libft/ft_strncmp.c`, `exec_utils.c:15-22` | 🟢 observado |
| BUG-21 | Baixa | [Revisão 2026-11-22] `exit 2 <arg>` encerra com 2 em vez de `too many arguments`: o valor 2 é confundido com o código de erro de `is_char_number` | `exit.c:51` | 🟢 observado |
