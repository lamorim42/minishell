# Analisar Linha de Comando

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Visão Geral
Transforma a linha crua aceita pelo laço interativo (já com aspas balanceadas e sem ser comentário) em uma sequência de **tokens** com uma **categoria léxica** paralela, valida a gramática e, depois da expansão, gera os **tokens limpos** sem aspas. É a ponte entre `iniciar-sessao-e-ler-comando` e a montagem da lista de execução 🟢.

Fronteira: esta unit define *como* os tokens são cortados, classificados, validados e limpos. A **ordem** "expandir → limpar aspas" e as regras de substituição de `$` ficam em `expandir-variaveis` 🟢.

## Responsabilidades
- Contar os tokens da linha antes de alocar os arrays (`tks_nbr`) 🟢
- Cortar a linha em tokens (`tks`), preservando as aspas dentro de cada token 🟢
- Classificar cada token em `WORD`, `VAR`, `PIPE`, `REDI`, `REDO`, `REDA` ou `HERE` (`lex`, array paralelo a `tks`) 🟢
- Validar a gramática sobre `lex` e, em caso de erro, informar `Syntax ERROR!` e descartar a linha 🟢
- Gerar `ctks`, cópia de `tks` sem aspas, usada pelo resto do pipeline 🟢

## Regras de Negócio

### Corte de tokens
- RN-06: só o espaço `' '` separa tokens. Tab e outros brancos fazem parte do token 🟢. ⚠️ No bash, tab também separa
- AT-01: `|`, `<` e `>` são tokens próprios, mesmo colados a palavras (`ls>file` → `ls`, `>`, `file`) 🟢
- AT-02: `>>` e `<<` são tokens de 2 caracteres. Sequências maiores são cortadas da esquerda para a direita, em pares: `<<<` → `<<`, `<`; `<>` → `<`, `>`; `>|` → `>`, `|`; `||` → `|`, `|` 🟢
- AT-03: um token que começa com aspa vai até a aspa de fechamento do mesmo tipo e continua até um delimitador do conjunto `| > < $ espaço`. Se encontrar outra aspa nesse trecho, absorve o novo trecho entre aspas: `"a"'b'c` é um token só 🟢
- AT-04: `$` depois de um trecho entre aspas **corta** o token: `"a"$X` → `"a"`, `$X` 🟢. ⚠️ No bash é uma palavra só
- AT-05: uma palavra que **não** começa com `$` absorve aspas: `a"b c"d` é um token só, com o espaço interno preservado 🟢
- AT-06: uma palavra que começa com `$` é cortada na primeira aspa: `$X'q'` → `$X`, `'q'`. Por isso `echo $X'q'` imprime dois argumentos (BUG-17) 🟢. ⚠️ No bash é `helloq`
- AT-07: dentro de aspas, operadores e espaços são literais: `'/bin/ls obj'` é um token só 🟢
- AT-08: a contagem (`count_tks`) segue exatamente as mesmas regras do corte; o número contado dimensiona `tks`, `lex` e `ctks` 🟢

### Classificação léxica (primeira regra que casar)
- LX-01: começa com `|` → `PIPE` 🟢
- LX-02: igual a `>>` → `REDA`; igual a `<<` → `HERE` 🟢
- LX-03: começa com `>` → `REDO`; começa com `<` → `REDI` 🟢
- LX-04: contém `$` e tem mais de 1 caractere → `VAR`, inclusive entre aspas (`"$HOME"`, `'$HOME'`, `a$b`) 🟢
- LX-05: qualquer outro token → `WORD`, incluindo `$` sozinho e operadores entre aspas (`">"` é `WORD`) 🟢

### Gramática
- RN-10 / G-01: `PIPE` na primeira posição é erro 🟢
- RN-10 / G-02: `PIPE` seguido de `PIPE` é erro (`ls | | cat`, `ls || cat`) 🟢
- RN-11 / G-03: `REDO`, `REDA`, `REDI` e `HERE` precisam ser seguidos **exatamente** de um `WORD`. Fim da linha, operador ou `VAR` são erro: `> $F`, `> "$x"`, `<< $EOF`, `> a$b` são rejeitados (BUG-13) 🟢. ⚠️ O bash aceita alvos com `$`
- RN-12: `PIPE` no fim da linha é aceito; `ls |` executa `ls` com a saída indo para um pipe que ninguém lê, sem mensagem 🟢. ⚠️ O bash pede continuação
- RN-13: erro de sintaxe → `Syntax ERROR!\n` em **stdout**, `tks`, `lex` e a linha são liberados, nada executa, a linha não entra no histórico e `$?` não muda (BUG-12) 🟢. ⚠️ O bash escreve em stderr e grava `$?=2`
- Linha só com espaços gera zero tokens, passa na gramática e segue para a execução sem efeito visível 🟢
- RN-14: não há suporte a `;`, `&&`, `||` (como operador lógico), `()`, `*`, `\` (escape) nem `<>`. Esses caracteres são literais ou caem nas regras acima 🟡

### Remoção de aspas
- AQ-01: token sem aspas é copiado como está 🟢
- AQ-02: token que começa **e** termina com `"` perde **todas** as `"` das duas pontas (`ft_strtrim`); as do meio ficam: `"a"b"c"` → `a"b"c` (BUG-11) 🟢. ⚠️ O bash dá `abc`
- AQ-03: a mesma regra vale para `'` nas duas pontas 🟢
- AQ-04: nos demais casos, **todas** as aspas dos dois tipos são removidas: `a"b'c"` → `abc` (BUG-11) 🟢. ⚠️ O bash dá `ab'c`
- AQ-05: aspas do tipo oposto dentro de aspas são preservadas só quando o token cai em AQ-02/AQ-03: `"it's"` → `it's` 🟢
- AQ-06: a limpeza roda sobre os tokens **já expandidos**, então aspas vindas do valor de uma variável também são removidas (RN-26) 🟢
- ✅ L-07 (decidida em 2026-11-22, [Revisão]): **documentar o comportamento atual**. As ⚠️ DIVERGÊNCIAS desta unit são o comportamento especificado, não defeitos a corrigir. (Pendência original: não está decidido se as ⚠️ DIVERGÊNCIAS acima devem ser preservadas (documentação as-is) ou corrigidas em uma reimplementação)

## Requisitos Funcionais

| ID | Requisito | Prioridade | Critério de Aceite |
|----|-----------|-----------|-------------------|
| RF-01 | Cortar a linha em tokens conforme AT-01 a AT-07 | Must | `ls -a\|cat -e` → `ls`, `-a`, `\|`, `cat`, `-e` |
| RF-02 | Contar os tokens com as mesmas regras do corte (AT-08) | Must | `count_tks(s)` igual ao número de elementos de `tokenizer(s)` para toda linha válida |
| RF-03 | Gerar `lex[]` paralelo a `tks[]`, terminado em `NULL`, conforme LX-01 a LX-05 | Must | `cat << miau` → `WORD`, `HERE`, `WORD` |
| RF-04 | Rejeitar a linha quando violar G-01, G-02 ou G-03 | Must | `\| ls`, `ls \| \| cat`, `ls >` → `Syntax ERROR!` |
| RF-05 | Aceitar `PIPE` no fim da linha (RN-12) | Should | `ls \|` não exibe erro |
| RF-06 | Em erro de sintaxe, liberar `tks`, `lex` e `str`, não executar nada e não mexer em `$?` | Must | Depois de `ls >`, `echo $?` mostra o status anterior |
| RF-07 | Gerar `ctks[]` sem aspas conforme AQ-01 a AQ-05 | Must | `echo "oi"` → `ctks` = `echo`, `oi` |
| RF-08 | Tratar operadores entre aspas como palavras literais | Must | `echo ">"` imprime `>` e não cria arquivo |

## Requisitos Não Funcionais

| Tipo | Requisito inferido | Evidência no código | Confiança |
|------|--------------------|---------------------|-----------|
| Testabilidade | Módulo coberto por 83 testes GoogleTest (tokenizer 22, count_tks 31, lexical 10, sintaxe 7, clean 13) | `tests/*.cpp`, ADR-002 | 🟢 |
| Desempenho | Duas passadas lineares sobre a linha (contar e cortar); a classificação é uma passada sobre os tokens | `src/tokens/tokenizer_utils.c:17-19` | 🟢 |
| Robustez | `lexical_analyzer` e `clean_tokens` não verificam o retorno do `malloc`; `tokenizer` retorna `NULL` em falha, mas `building_tokens` não trata | `src/tokens/lexical_analyzer.c:23`, `src/tokens/clean_tokens.c:25`, `src/tokens/tokenizer_utils.c:18` | 🟢 |
| Pré-condição | O corte presume aspas balanceadas; o laço interativo garante isso antes (`verify_quots`) | `src/main.c`, `src/tokens/sintax_analysis.c:46` | 🟢 |

## Critérios de Aceitação

```gherkin
Funcionalidade: Análise da linha de comando

  Cenário: Operadores colados a palavras
    Dado a linha "ls>>file"
    Quando a linha é tokenizada
    Então os tokens são "ls", ">>", "file"
    E as categorias são WORD, REDA, WORD

  Cenário: Texto entre aspas forma um token só
    Dado a linha "echo 'a | b'"
    Quando a linha é analisada
    Então os tokens são "echo", "'a | b'"
    E nenhuma categoria é PIPE

  Cenário: Palavra que absorve aspas
    Dado a linha 'echo a"b c"d'
    Quando a linha é analisada e limpa
    Então há 2 tokens
    E o segundo token limpo é "ab cd"

  Cenário: Pipe no início
    Dado a linha "| ls"
    Quando a gramática é validada
    Então "Syntax ERROR!" é escrito em stdout
    E nenhum comando é executado
    E $? mantém o valor anterior

  Cenário: Redirecionamento sem alvo
    Dado a linha "echo oi >"
    Quando a gramática é validada
    Então "Syntax ERROR!" é escrito em stdout
    E nenhum arquivo é criado

  Cenário: Operador entre aspas é literal
    Dado a linha 'echo ">"'
    Quando a linha é executada
    Então ">" é exibido
    E nenhum arquivo é criado

  @divergente
  Cenário: Pipe no fim é aceito (bash pediria continuação)
    Dado a linha "ls |"
    Quando a linha é executada
    Então nenhuma mensagem de erro é exibida

  @divergente
  Cenário: Alvo de redirecionamento com variável é rejeitado (BUG-13)
    Dado a linha "echo oi > $HOME/f"
    Quando a gramática é validada
    Então "Syntax ERROR!" é escrito em stdout

  @divergente
  Cenário: Variável seguida de aspas vira dois argumentos (BUG-17)
    Dado X=hello no ambiente
    Quando o usuário digita "echo $X'q'"
    Então a saída é "hello q"

  @divergente
  Cenário: Remoção de aspas pelas pontas (BUG-11)
    Quando o usuário digita 'echo "a"b"c"'
    Então a saída é 'a"b"c'
```

## Prioridade (MoSCoW)

| Requisito | MoSCoW | Justificativa |
|-----------|--------|---------------|
| RF-01, RF-02, RF-03 | Must | Toda linha não vazia passa por aqui; os arrays dimensionam o resto do pipeline |
| RF-04, RF-06 | Must | Impede montar uma lista de execução inválida |
| RF-07, RF-08 | Must | Sem eles, argumentos com aspas chegam errados aos comandos |
| RF-05 | Should | Comportamento herdado e sem efeito útil; mantido por fidelidade ao as-is |

> Prioridade inferida pela posição no pipeline (`facade`) e pela cobertura de testes.

## Rastreabilidade de Código

| Arquivo | Função | Cobertura |
|---------|--------|-----------|
| `src/tokens/tokenizer_utils.c` | `building_tokens`, `ft_strcmp_len` | 🟢 |
| `src/tokens/count_tks.c` | `count_tks`, `count_special_chr`, `count_inside_quot_as_one_word`, `count_word` | 🟢 |
| `src/tokens/tokenizer.c` | `tokenizer`, `worddup`, `make_red_len`, `make_quots_len`, `make_word_len` | 🟢 |
| `src/tokens/lexical_analyzer.c` | `lexical_analyzer`, `find_lexema` | 🟢 |
| `src/tokens/sintax_analysis.c` | `sintax_analysis` | 🟢 |
| `src/tokens/clean_tokens.c` | `clean_tokens`, `clean_quots`, `clean_in_middle`, `count_quots` | 🟢 |
| `src/free.c` | `free_sintax` | 🟢 |
| `src/facade.c` | `facade` (chamada de `building_tokens` e `sintax_analysis`), `exec_pipe_line` (chamada de `clean_tokens`) | 🟢 |
| `tests/tests_*.cpp` | Suítes GTest do módulo | 🟢 |
