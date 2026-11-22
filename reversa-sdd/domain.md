# Domínio — minishell (MiauShell)

> Gerado pelo **Detetive** (Reversa) em 2026-11-11
> Fontes: código (`modules.json`, `code-analysis.md`), 424 commits do Git, README e execuções do binário isolado.
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA

---

## 1. Contexto de negócio

O minishell é um **projeto acadêmico da 42 São Paulo**. O objetivo é reproduzir um subconjunto do bash: prompt interativo, execução de programas, pipes, redirecionamentos, heredoc, variáveis de ambiente, `$?`, sete builtins e tratamento de Ctrl-C, Ctrl-D e Ctrl-\ 🟢 (README).

O "cliente" é a banca de avaliação da 42. Isso explica várias decisões que não são funcionais:
- **Norminette:** funções curtas, no máximo 5 variáveis, uma única variável global. Os commits `refactor: norminette`, `NORMINETTE OK` e `put in norma` mostram isso 🟢.
- **Lista fechada de funções autorizadas da libc,** documentada no README 🟢.
- **Libft obrigatória** como biblioteca de apoio 🟢.

O desenvolvimento foi feito em dupla, Daniela Monteiro (`dmonteir`) e Luís Amorim (`lamorim`), entre 16/03/2022 e 25/07/2022. O trabalho foi organizado em PRs por feature (#6 a #35) 🟢.

O README apresenta a referência funcional assim: *"replicar um bash com algumas funcionalidades builtins"*. **O bash é o oráculo implícito de comportamento** 🟡. O enunciado oficial da 42 (versão do subject) **não está no repositório** 🔴.

---

## 2. Glossário

| Termo | Definição no sistema | Fonte |
|-------|----------------------|-------|
| **Linha** (`t_line`) | Unidade de trabalho: o texto digitado depois do prompt e todo o estado derivado dele | `minishell.h` 🟢 |
| **Token** (`tks`) | Pedaço da linha separado por espaços e operadores, ainda com aspas | `tokenizer.c` 🟢 |
| **Token limpo** (`ctks`) | Token sem aspas, depois da expansão | `clean_tokens.c` 🟢 |
| **Lexema / categoria** (`lex`) | Classe do token: `WORD`, `VAR`, `PIPE`, `REDI`, `REDO`, `REDA`, `HERE` | `lexical_analyzer.c` 🟢 |
| **WORD** | Palavra literal | 🟢 |
| **VAR** | Palavra com `$` que precisa de expansão | 🟢 |
| **PIPE** | Operador `\|` | 🟢 |
| **REDI** | *Redirect Input*: `<` | 🟢 |
| **REDO** | *Redirect Output*: `>` (trunca) | 🟢 |
| **REDA** | *Redirect Append*: `>>` | 🟢 |
| **HERE** | *Here-document*: `<<` | 🟢 |
| **Nó** (`t_pipe_list`) | Elemento da lista de execução: um **comando** ou um **operador** com seu alvo | `make_list.c` 🟢 |
| **Comando** | Nó cujo `args[0]` não é categoria de operador | `is_command` 🟢 |
| **bin** | Caminho absoluto do executável, o literal `"builtin"`, ou `NULL` | `list_generation_bin.c` 🟢 |
| **Builtin** | Comando interno: `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit` | `builtins_utils.c` 🟢 |
| **Tabela** (`t_hash_table`) | O "ambiente" do shell: variáveis exportadas e `?` | `hash_table/` 🟢 |
| **Status code** / `$?` | Código de saída do último comando, guardado na chave `"?"` da tabela | `exec_list.c` 🟢 |
| **Heredoc** | Texto digitado até uma linha delimitadora, entregue como stdin | `here_doc*.c` 🟢 |
| **Delimitador** | `args[1]` do nó `HERE`, já sem aspas | 🟢 |
| **Facade** | Função `facade()`, ponto único que orquestra o processamento de uma linha | README, `facade.c` 🟢 |
| **Miau** | Identidade do projeto: prompt `miau> `, nome *MiauShell*, "feito por pessoas que amam gatos" | README 🟢 |
| **Sorvetinho / saga do gnl** | Mensagens de commit informais (07/07 e 11/07/2022). Não revelam decisão 🟢 | Git |

---

## 3. Regras de negócio

As regras estão agrupadas por capacidade. Os IDs `RN-*` são estáveis para rastreabilidade. A coluna **Bash?** indica se o comportamento coincide com o bash (o oráculo implícito).

### 3.1 Entrada e prompt

| ID | Regra | Bash? | Confiança |
|----|-------|-------|-----------|
| RN-01 | O prompt é `miau> ` | — | 🟢 |
| RN-02 | Ctrl-D com a linha vazia escreve `exit` em stderr e encerra com 0 | ≈ (o bash usa o último `$?`) | 🟢 |
| RN-03 | Aspas desbalanceadas → `Syntax ERROR!` em stdout. Nada executa e `$?` não muda | ✗ (o bash pede continuação) | 🟢 |
| RN-04 | Linha que começa com `#` (depois de espaços) é comentário | ✓ parcial (o bash também aceita `#` no meio) | 🟢 |
| RN-05 | Só linhas executadas entram no histórico | ✗ | 🟢 |
| RN-06 | Só o espaço separa tokens. Tab não é separador | ✗ | 🟢 |

### 3.2 Gramática

| ID | Regra | Bash? | Confiança |
|----|-------|-------|-----------|
| RN-10 | `\|` no início ou `\|\|` → erro de sintaxe | ✓ | 🟢 |
| RN-11 | Operador de redirecionamento precisa ser seguido de uma palavra literal. Alvo com `$` é rejeitado | ✗ | 🟢 |
| RN-12 | `\|` no fim da linha é aceito | ✗ | 🟢 |
| RN-13 | Erro de sintaxe → `Syntax ERROR!` em stdout, `$?` não muda | ✗ (bash: stderr, `$?=2`) | 🟢 |
| RN-14 | Não há suporte a `;`, `&&`, `\|\|`, `()`, `*`, `\` (escape), `<>` | ✓ (fora do escopo obrigatório da 42) | 🟡 |

### 3.3 Aspas e expansão

| ID | Regra | Bash? | Confiança |
|----|-------|-------|-----------|
| RN-20 | Aspas simples impedem a expansão, **mas só se o token começar com `'`** | parcial | 🟢 |
| RN-21 | `$NOME` é substituído pelo valor na tabela | ✓ | 🟢 |
| RN-22 | `$?` expande para o status da última execução | ✓ | 🟢 |
| RN-23 | Variável inexistente **apaga o token inteiro** | ✗ | 🟢 |
| RN-24 | O nome da variável vai até um delimitador do conjunto `"',:./[]{}=-+*!@# `. `$` não delimita | ✗ | 🟢 |
| RN-25 | Não há divisão de palavras depois da expansão | ✗ | 🟡 |
| RN-26 | As aspas são removidas depois da expansão, inclusive as que vêm do valor | ✗ | 🟢 |
| RN-27 | O heredoc não expande variáveis | ✗ (o bash expande quando o delimitador não tem aspas) | 🟢 |

### 3.4 Execução

| ID | Regra | Bash? | Confiança |
|----|-------|-------|-----------|
| RN-30 | Comando com `/`, `./` ou `../` no início é executado pelo caminho literal | ≈ (o bash aceita qualquer `/`) | 🟢 |
| RN-31 | Os outros comandos são procurados nos diretórios de `PATH`, na ordem, e o primeiro que **existe** é escolhido | ≈ (o bash exige `X_OK`) | 🟢 |
| RN-32 | Sem `PATH` → nenhum comando por nome é encontrado | ✓ | 🟢 (commit `fix: unset no PATH`) |
| RN-33 | Não encontrado → `<cmd>: command not found`, `$?=127` | ✓ | 🟢 |
| RN-34 | Os comandos de um pipeline rodam em paralelo; `$?` é o status do **último** | ✓ | 🟢 |
| RN-35 | Processo morto por sinal N → `$? = 128+N` | ✓ | 🟢 |
| RN-36 | Builtin isolado (último nó da linha) roda no shell; o resto roda em um filho | ≈ (bash: builtin sozinho, sem pipe) | 🟢 |
| RN-37 | Programas executados recebem o **ambiente original** | ✗ | 🟢 |

### 3.5 Redirecionamentos e heredoc

| ID | Regra | Bash? | Confiança |
|----|-------|-------|-----------|
| RN-40 | `>` cria ou trunca o arquivo com permissão `0644`; `>>` adiciona no fim | ✓ | 🟢 |
| RN-41 | Todos os arquivos de saída da linha são criados **antes** de qualquer execução | ≈ | 🟢 |
| RN-42 | Com vários redirecionamentos de saída, o último vence (e os outros são criados vazios) | ✓ | 🟢 |
| RN-43 | O redirecionamento explícito tem prioridade sobre o pipe | ✓ | 🟢 |
| RN-44 | Arquivo de entrada inexistente (depois do comando) → `<arq>: No such file or directory`, `$?=1` | ✓ | 🟢 |
| RN-45 | O heredoc lê linhas com o prompt `> ` até uma linha **idêntica** ao delimitador | ✓ | 🟢 |
| RN-46 | EOF no heredoc → `<delim>: Wanted` e o conteúdo lido até ali é usado | ≈ (o bash avisa e usa o conteúdo) | 🟢 |
| RN-47 | Todos os heredocs da linha são lidos **antes** de qualquer comando executar | ✓ | 🟢 |

### 3.6 Builtins

| ID | Regra | Confiança |
|----|-------|-----------|
| RN-50 | `echo [-n ...] args`: imprime os argumentos separados por espaço; `-n` (exato, pode repetir no início) suprime o `\n` | 🟢 |
| RN-51 | `cd [dir\|~]`: sem argumento ou com `~` vai para `HOME`; atualiza `PWD` e `OLDPWD`; erro → mensagem do sistema e `$?=1` | 🟢 |
| RN-52 | `pwd`: imprime `PWD` da tabela | 🟢 |
| RN-53 | `export`: sem argumento lista `declare -x K="V"`; com `K=V` define. O identificador deve começar com letra | 🟢 |
| RN-54 | `unset K...`: remove as variáveis; identificador inválido → `not a valid identifier`, `$?=1` | 🟢 |
| RN-55 | `env`: lista `K=V`; com argumento → erro e 127 | 🟢 |
| RN-56 | `exit [n]`: encerra com `n` (ou 0); não numérico → 2; dois ou mais argumentos → erro sem sair | 🟢 |

### 3.7 Sinais

| ID | Regra | Confiança |
|----|-------|-----------|
| RN-60 | Ctrl-C no prompt: descarta a linha e mostra um novo prompt | 🟢 |
| RN-61 | Ctrl-\ no prompt: ignorado | 🟢 |
| RN-62 | Ctrl-C durante a execução: interrompe o comando, `$?=130` | 🟢 |
| RN-63 | Ctrl-\ durante a execução: imprime `Quit` e `$?=131` | 🟢 |
| RN-64 | Ctrl-C no heredoc: interrompe a leitura (o filho sai com 130) | 🟢 |
| RN-65 | O pai não usa o status 130 do heredoc para cancelar a linha | 🟡 |

---

## 4. Intenções reveladas pelo Git

| Evidência (commit) | Intenção revelada |
|--------------------|-------------------|
| `2022-03-21 refactor: REMOVE all linked-list and stack files` / `start new approach` | A primeira abordagem (pilha + lista ligada) foi descartada → ADR-003 🟢 |
| `2022-04-11 Init_tests_with_tdd`, `2022-04-19 tests: New framework test (gtest)` | Adoção de TDD, primeiro em C e depois migrado para GoogleTest → ADR-002 🟢 |
| `2022-05-11 transforming linked list in doubly linkd list` | O `prev` foi adicionado para olhar o nó anterior (pipe ou redirecionamento) → ADR-005 🟢 |
| `2022-05-16/24 init HashTable` | Ambiente próprio em hash table → ADR-004 🟢 |
| `2022-07-06 refactor/list-without-cmd-table` (PR #26) | A tabela de comandos (`char ***cmds`) foi substituída pela lista de nós → ADR-005 🟢 |
| `2022-07-03 init here_doc`, `07-17 fix: here_doc signals` | Heredoc em processo filho, para isolar o Ctrl-C → ADR-006 🟢 |
| `2022-07-23 putting global conditions` | A global única `g_minishell` foi criada no último dia, para cumprir a norma e dar acesso aos handlers → ADR-007 🟢 |
| `2022-07-23 fix: unset no PATH` | A resolução de `PATH` passou do `envp` para a tabela, para que `unset PATH` funcione → ADR-008 🟢 |
| `2022-07-23 fix: unset no PWD e no HOME`, `fix: unset OLDPWD` | Foram testados casos de borda de unset em variáveis críticas 🟢 |
| `2022-07-18 add minishell_tester in .gitignore` | Uso de um testador externo (provavelmente um *minishell tester* da comunidade 42) 🟡 |
| `2022-03-30 change gcc to clang` | O compilador foi padronizado para clang (padrão das máquinas da 42) 🟡 |
| `2022-07-21 fix: single_quouts expand`, `07-23 fix: echo 'oi'$USER'oi'` | Os casos de aspas misturadas com variáveis foram corrigidos caso a caso, sem um modelo formal de *quoting* 🟡 |
| Nenhum `TODO`/`FIXME` no código | — 🟢 |

**Leitura geral:** os 3 últimos dias (21 a 23/07/2022) concentram muitas correções pontuais e um refactor de estilo. Isso é típico da preparação para a avaliação. A ausência de commits depois de 25/07 sugere que o projeto foi **entregue e encerrado** 🟡.

---

## 5. Lacunas (🔴, precisam de validação humana)

| ID | Pergunta |
|----|----------|
| L-01 | Qual versão do *subject* da 42 o projeto seguiu? O comportamento-alvo é "igual ao bash" ou só o mínimo do subject? |
| L-02 | O projeto foi aprovado na avaliação? Com qual nota? (Isso indica se os bugs da §9 do `code-analysis.md` eram conhecidos ou aceitos.) |
| L-03 | `export`/`unset` não chegarem aos filhos (RN-37) é intencional ou um bug? |
| L-04 | `Syntax ERROR!` em stdout, sem `$?=2`, foi uma escolha deliberada? |
| L-05 | Qual era o `minishell_tester` usado (repositório/URL)? |
| L-06 | `readline.supp` (supressões do Valgrind) existia localmente. Vale versionar? |
| L-07 | Qual o objetivo desta engenharia reversa: documentar, corrigir os bugs, reescrever ou migrar? (Isso orienta o Redator e o Revisor.) |

### Respostas às lacunas ([Revisão] 2026-11-22, `questions.md`)

| ID | Resposta |
|----|----------|
| L-01 | O alvo é o **mínimo exigido pelo subject**. Recursos além dele (`${}`, `$$`, `cd -`, `-c`, tab como separador, *word splitting*) ficam fora do escopo |
| L-02 | O projeto foi **aprovado**; os bugs eram conhecidos |
| L-03 | `export`/`unset` não chegarem aos filhos é um **bug** (BUG-03) |
| L-04 | `Syntax ERROR!` em stdout sem `$?=2` **não foi deliberado**; é bug |
| L-05 | Os resultados do `minishell_tester` não foram guardados; não há oráculo externo |
| L-06 | O `readline.supp` não será versionado |
| L-07 | **Documentar o comportamento atual**: as ⚠️ DIVERGÊNCIAS são o comportamento especificado |
| P-7 | Numa reimplementação, os casos que travam ou derrubam o shell (RX-06, BUG-01, RE-09) devem ser **corrigidos** |
| P-8 | BUG-20 e BUG-21 foram acrescentados à §9 de `code-analysis.md` |
