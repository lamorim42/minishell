# Expandir Variáveis

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash
> Casos marcados "(observado)" foram executados contra o código legado compilado na scratchpad, com um driver que chama `building_tokens` → `expand_var` → `clean_tokens`. O repositório não foi alterado.

## Visão Geral
Substitui as referências `$NOME` e `$?` dentro dos tokens pelos valores da tabela de ambiente, depois de a gramática ser validada e antes da remoção de aspas. O resultado alimenta `clean_tokens` (unit `analisar-linha-de-comando`) e, a partir dele, os argumentos dos comandos 🟢.

## Responsabilidades
- Selecionar os tokens que podem ser expandidos (categoria `VAR` e que não começam com `'`) 🟢
- Extrair o nome da variável de cada `$` 🟢
- Buscar o valor na tabela hash, inclusive a chave especial `?` 🟢
- Reescrever o token no lugar (`tks[i]`), repetindo até não restar `$` 🟢
- Garantir a ordem **expandir → remover aspas** 🟢

## Regras de Negócio

### Seleção do token
- RX-01: só são expandidos tokens com lexema exatamente `VAR` (contêm `$` e têm mais de 1 caractere) 🟢
- RN-20: um token que **começa** com `'` não é expandido: `'$HOME'` → `$HOME` 🟢 (observado)
- RN-20 (parcial): aspas simples que **não** estão no início do token não protegem: `a'$HOME'` → `a/home/u` 🟢 (observado). ⚠️ O bash dá `a$HOME`
- Aspas simples dentro de aspas duplas são literais e o conteúdo é expandido: `"'$HOME'"` → `'/home/u'` 🟢 (observado), igual ao bash
- `$` sozinho é `WORD` e fica literal: `echo $` → `$` 🟢 (observado)
- Alvos de redirecionamento e delimitadores de heredoc nunca chegam aqui, porque a gramática rejeita `VAR` depois de operador (BUG-13): `cat << $EOF` → `Syntax ERROR!` 🟢 (observado)

### Nome da variável
- RN-24: o nome começa logo depois do `$` e vai até o primeiro caractere do conjunto `" ' , : . / [ ] { } = - + * ! @ #` ou espaço, ou até o fim do token 🟢
- `$` **não** é delimitador: em `$HOME$USER` o nome é `HOME$USER`, que não existe, e o token vira vazio (BUG-05) 🟢 (observado). ⚠️ O bash concatena os dois valores
- `_`, letras, dígitos e `?` fazem parte do nome: `$X_Y` procura `X_Y`; `$?abc` procura `?abc` e o token vira vazio 🟢 (observado). ⚠️ No bash, `$?abc` → `<status>abc`
- `${NOME}` não é suportado: `{` é delimitador, o nome fica vazio e o token vira vazio 🟢 (observado). ⚠️ O bash suporta
- Parâmetros posicionais e especiais (`$1`, `$$`, `$!`) não existem; viram vazio ou seguem as regras acima 🟢 (observado para `$1`)

### Substituição
- RN-21: `$NOME` existente → o trecho `$NOME` é trocado pelo valor e o prefixo e o sufixo do token são preservados: `pre$HOME/pos` → `pre/home/u/pos` 🟢 (observado)
- Vários `$` separados por delimitadores no mesmo token são expandidos um por um, da esquerda para a direita: `"$HOME $USER"` → `/home/u luis`; `$HOME.$USER` → `/home/u.luis` 🟢 (observado)
- RN-22: `$?` → valor da chave `?` da tabela, gravada por `exec_list` ao fim de cada execução (`"?"=0` no início da sessão) 🟢
- RN-23 / RX-02: variável **inexistente** → o **token inteiro** vira `""`, com prefixo, sufixo e outras variáveis: `abc$NOPE` → `""`; `"a $NOPE b"` → `""` (BUG-05) 🟢 (observado). ⚠️ O bash remove só o `$NOPE`
- Variável existente com valor vazio (`Z=`) → o trecho vira `""` e o resto é preservado 🟢 (observado)
- O token vazio **continua** na lista de argumentos como `""`; o `echo` ignora argumentos vazios, mas um comando externo os recebe: `ls $NOPE` → `ls: cannot access '': No such file or directory`, `$?=2` 🟢 (observado na revisão de 2026-11-22). ⚠️ O bash descarta a palavra
- RX-06: se o valor contém `$`, ele é expandido de novo: `D='x$USER'` → `xluis` 🟢 (observado). ⚠️ O bash não reexpande valores
- RX-06 (falha): um valor que se refere a si mesmo (`A='$A'`, `B='-$B'`) causa **loop infinito**; o shell trava 🟢 (observado: processo morto pelo `timeout` depois de 3 s)
- RN-25 / RX-07: não há divisão de palavras: `V='a b'` → `echo $V` passa **um** argumento `a b` 🟢 (observado). ⚠️ O bash passaria dois

### Interação com as aspas
- RN-26: a remoção de aspas roda **depois** da expansão, então as aspas vindas do valor também são removidas: `Q='"q"'` → `echo $Q` → `q` 🟢 (observado). ⚠️ O bash imprime `"q"`
- `"$X"q` → um token, `helloq` 🟢 (observado), igual ao bash
- `$X"q"` e `$X'q'` → dois tokens, `hello` e `q` (BUG-17, corte na tokenização) 🟢 (observado). ⚠️ O bash dá `helloq`
- RN-27: o heredoc não expande variáveis (detalhado em `ler-heredoc`) 🟢
- ✅ L-07 (decidida em 2026-11-22, [Revisão]): **documentar o comportamento atual**. As ⚠️ DIVERGÊNCIAS desta unit são o comportamento especificado, não defeitos a corrigir. (Pendência original: não está decidido se as ⚠️ DIVERGÊNCIAS acima devem ser preservadas ou corrigidas)

## Requisitos Funcionais

| ID | Requisito | Prioridade | Critério de Aceite |
|----|-----------|-----------|-------------------|
| RF-01 | Expandir `$NOME` em tokens `VAR` que não começam com `'` | Must | `echo $HOME` imprime o diretório do usuário |
| RF-02 | Não expandir tokens que começam com `'` | Must | `echo '$HOME'` imprime `$HOME` |
| RF-03 | Expandir `$?` com o status da última execução | Must | `false; echo $?` → `1` (em linhas separadas) |
| RF-04 | Preservar prefixo e sufixo do token quando a variável existe | Must | `echo pre$HOME/pos` → `pre/home/u/pos` |
| RF-05 | Expandir todas as referências separadas por delimitador no mesmo token | Must | `echo "$HOME $USER"` mostra os dois valores |
| RF-06 | Trocar o token inteiro por `""` quando uma variável não existe (as-is, BUG-05) | Should | `echo abc$NOPE` imprime linha vazia |
| RF-07 | Executar a expansão antes da remoção de aspas | Must | `echo "$HOME"` → sem aspas e expandido |
| RF-08 | Não dividir palavras depois da expansão | Should | `V='a b'`; `echo $V` passa 1 argumento |

## Requisitos Não Funcionais

| Tipo | Requisito inferido | Evidência no código | Confiança |
|------|--------------------|---------------------|-----------|
| Disponibilidade | Não há limite de reexpansão; valor auto-referente trava o shell | `src/exec/expand_var.c:27-28` | 🟢 (observado) |
| Desempenho | Cada `$` faz uma busca na tabela hash (149 buckets) e reconstrói o token com `ft_strjoin` | `src/exec/expand_var.c:39-46`, `src/hash_table/search_item.c:15-31` | 🟢 |
| Memória | O valor devolvido por `search_item` é uma cópia (`ft_strdup`) e é liberado depois da junção | `src/hash_table/search_item.c:26`, `src/exec/expand_var.c:46` | 🟢 |

## Critérios de Aceitação

```gherkin
Funcionalidade: Expansão de variáveis

  Cenário: Variável simples
    Dado HOME=/home/u no ambiente
    Quando o usuário digita "echo $HOME"
    Então a saída é "/home/u"

  Cenário: Variável entre aspas duplas com várias referências
    Dado HOME=/home/u e USER=luis
    Quando o usuário digita 'echo "$HOME $USER"'
    Então a saída é "/home/u luis"

  Cenário: Aspas simples impedem a expansão
    Quando o usuário digita "echo '$HOME'"
    Então a saída é "$HOME"

  Cenário: Status da última execução
    Dado que o último comando terminou com status 127
    Quando o usuário digita "echo $?"
    Então a saída é "127"

  Cenário: Prefixo e sufixo preservados
    Dado HOME=/home/u
    Quando o usuário digita "echo pre$HOME/pos"
    Então a saída é "pre/home/u/pos"

  Cenário: Cifrão isolado é literal
    Quando o usuário digita "echo $"
    Então a saída é "$"

  @divergente
  Cenário: Variável inexistente apaga o token inteiro (BUG-05)
    Quando o usuário digita 'echo "a $NOPE b"'
    Então a saída é uma linha vazia

  @divergente
  Cenário: Duas variáveis coladas (BUG-05)
    Dado HOME=/home/u e USER=luis
    Quando o usuário digita "echo $HOME$USER"
    Então a saída é uma linha vazia

  @divergente
  Cenário: Aspas simples no meio do token não protegem
    Dado HOME=/home/u
    Quando o usuário digita "echo a'$HOME'"
    Então a saída é "a/home/u"

  @divergente
  Cenário: Aspas vindas do valor são removidas
    Dado Q='"q"' no ambiente
    Quando o usuário digita "echo $Q"
    Então a saída é "q"

  @divergente @trava
  Cenário: Valor auto-referente
    Dado A='$A' no ambiente
    Quando o usuário digita "echo $A"
    Então o shell entra em loop infinito
```

## Prioridade (MoSCoW)

| Requisito | MoSCoW | Justificativa |
|-----------|--------|---------------|
| RF-01, RF-02, RF-03, RF-07 | Must | Exigidos pelo subject; `$?` é o único canal de status para o usuário |
| RF-04, RF-05 | Must | Usados em praticamente todo comando com variável (`cd $HOME/x`, `export A=$B`) |
| RF-06, RF-08 | Should | Comportamento herdado e divergente; mantido por fidelidade ao as-is (L-07 = documentar o comportamento atual) |

> Prioridade inferida pela posição no pipeline (`exec_pipe_line`) e pelo uso nos builtins.

## Rastreabilidade de Código

| Arquivo | Função | Cobertura |
|---------|--------|-----------|
| `src/exec/expand_var.c` | `expand_var`, `change_value`, `join_str_value`, `join_helper` | 🟢 |
| `src/exec/expand_var_utils.c` | `return_var_key`, `key_len` | 🟢 |
| `src/exec/exec_utils.c` | `take_prefix_var`, `sufix_var` | 🟢 |
| `src/hash_table/search_item.c` | `search_item` | 🟢 |
| `src/exec/exec_list.c` | atualização de `?` (linhas 66-67) | 🟢 |
| `src/tokens/tokenizer_utils.c` | `ft_strcmp_len` (teste do lexema `VAR`) | 🟢 |
| `src/facade.c` | `exec_pipe_line` (ordem `expand_var` → `clean_tokens`) | 🟢 |
