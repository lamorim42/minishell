# Ler Heredoc

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash
> Casos "(observado)" foram executados contra o `main.c` legado compilado na scratchpad e conduzido por um pseudo-terminal (`pty`), com um `readline` substituto. O repositório não foi alterado.

## Visão Geral
Implementa `<< DELIM`: lê linhas do usuário com o prompt `> ` até uma linha igual ao delimitador e entrega o conteúdo como stdin do comando. A leitura de **todos** os heredocs da linha acontece em um processo filho, antes de qualquer comando ser executado 🟢.

## Responsabilidades
- Criar um `pipe()` para cada nó `HERE` 🟢
- Ler o conteúdo de cada heredoc, na ordem da linha, em um processo filho 🟢
- Escrever o conteúdo no pipe e fechar as pontas 🟢
- Tratar EOF e Ctrl-C durante a leitura 🟢
- Entregar a ponta de leitura como stdin do comando (via `redirecionar-entrada-e-saida`) 🟢

## Regras de Negócio
- HD-01: o prompt de continuação é `> ` 🟢
- HD-02: a leitura termina na linha **exatamente igual** ao delimitador (mesmo tamanho e mesmo conteúdo) 🟢 (observado)
- RE-10: o delimitador é o alvo já **sem aspas**: `<< "F I"` termina na linha `F I` 🟢 (observado)
- RN-27 / RE-10: o conteúdo **nunca** é expandido, com ou sem aspas no delimitador: `a $HOME` sai literal 🟢 (observado). ⚠️ O bash expande quando o delimitador não tem aspas
- HD-03: as aspas do conteúdo são preservadas: `'q' "x"` → `'q' "x"` 🟢 (observado), igual ao bash
- HD-04: cada linha recebe `\n` no fim; o conteúdo vazio (delimitador na primeira linha) gera stdin vazio 🟢
- HD-05: vários heredocs na mesma linha são lidos **em sequência**; o último antes do próximo `|` vence como stdin: `cat << A << B` lê os dois e imprime só o de `B` 🟢 (observado), igual ao bash
- HD-06: `<< X` sem comando lê o conteúdo e o descarta, `$?=0` 🟢 (observado)
- HD-07: combina com pipe e com redirecionamento de saída: `cat << FIM | wc -l` → `2`; `cat << FIM > o_hd` grava o conteúdo 🟢 (observado)
- HD-08: EOF (Ctrl-D) durante a leitura → `<DELIM>: Wanted` em stderr; o que já foi lido é **mantido** e o comando executa 🟢 (observado). ⚠️ O bash escreve `warning: here-document delimited by end-of-file (wanted 'DELIM')`
- RE-08 / BUG-15: Ctrl-C durante a leitura encerra o filho do heredoc com 130, mas o shell **continua** e executa o comando com stdin vazio; `$?` termina com o status do comando (`0` para `cat`) 🟢 (observado). ⚠️ O bash cancela a linha inteira e grava `$?=130`
- RE-09: o conteúdo fica num `pipe()` lido só depois; conteúdo maior que a capacidade do pipe (~64 KiB no Linux) trava o shell: heredoc de 100.000 bytes → processo morto pelo `timeout` depois de 5 s 🟢 (observado na revisão de 2026-11-22)
- O delimitador não pode ser `$VAR` (a gramática rejeita, BUG-13) 🟢 (observado)
- ✅ L-07 (decidida em 2026-11-22, [Revisão]): **documentar o comportamento atual**. As ⚠️ DIVERGÊNCIAS desta unit são o comportamento especificado, não defeitos a corrigir. (Pendência original: RN-27 (expansão), BUG-15 (Ctrl-C) e RE-09 (limite do pipe))

## Requisitos Funcionais

| ID | Requisito | Prioridade | Critério de Aceite |
|----|-----------|-----------|-------------------|
| RF-01 | Ler linhas com `> ` até a linha igual ao delimitador | Must | `cat << FIM` + `a` + `FIM` → `a` |
| RF-02 | Usar o delimitador sem aspas | Must | `cat << "F I"` termina em `F I` |
| RF-03 | Não expandir variáveis no conteúdo (as-is) | Should | `$HOME` sai literal |
| RF-04 | Ler todos os heredocs da linha antes de executar | Must | `cat << A << B` pede os dois conteúdos antes de imprimir |
| RF-05 | Usar o último heredoc de cada comando como stdin | Must | `cat << A << B` imprime o conteúdo de `B` |
| RF-06 | Em EOF, avisar `<DELIM>: Wanted` e usar o conteúdo lido | Should | Ctrl-D depois de `linha` → mensagem e `linha` |
| RF-07 | Em Ctrl-C, encerrar a leitura | Must | Ctrl-C volta ao prompt (as-is: o comando ainda roda) |

## Requisitos Não Funcionais

| Tipo | Requisito inferido | Evidência no código | Confiança |
|------|--------------------|---------------------|-----------|
| Isolamento | A leitura roda em um filho; o pai ignora SIGINT/SIGQUIT enquanto espera | `src/facade.c:40-51`, `src/signals.c:33-38` | 🟢 |
| Capacidade | Conteúdo limitado pela capacidade do pipe, porque o filho escreve tudo antes de alguém ler | `src/exec/here_doc_utils.c:34-40` | 🟢 (observado na revisão de 2026-11-22) |
| Memória | Cada linha é concatenada com `ft_strjoin` (custo quadrático no tamanho) | `src/exec/here_doc.c:68-80` | 🟢 |

## Critérios de Aceitação

```gherkin
Funcionalidade: Heredoc

  Cenário: Conteúdo literal
    Quando o usuário digita "cat << FIM"
    E digita "a $HOME" e "FIM"
    Então a saída é "a $HOME"

  Cenário: Delimitador com aspas
    Quando o usuário digita 'cat << "F I"'
    E digita "z" e "F I"
    Então a saída é "z"

  Cenário: Dois heredocs
    Quando o usuário digita "cat << A << B"
    E digita "um", "A", "dois", "B"
    Então a saída é "dois"

  Cenário: Heredoc com pipe
    Quando o usuário digita "cat << FIM | wc -l"
    E digita "a", "b", "FIM"
    Então a saída é "2"

  Cenário: EOF antes do delimitador
    Quando o usuário digita "cat << FIM"
    E digita "linha" e envia Ctrl-D
    Então "FIM: Wanted" é escrito em stderr
    E a saída é "linha"

  @divergente
  Cenário: Ctrl-C não cancela o comando (BUG-15)
    Quando o usuário digita "cat << FIM"
    E digita "linha" e envia Ctrl-C
    Então o cat executa com entrada vazia
    E $? é 0

  @divergente
  Cenário: Sem expansão mesmo sem aspas (RN-27)
    Dado HOME=/home/u
    Quando o usuário digita "cat << FIM", "$HOME", "FIM"
    Então a saída é "$HOME"
```

## Prioridade (MoSCoW)

| Requisito | MoSCoW | Justificativa |
|-----------|--------|---------------|
| RF-01, RF-02, RF-04, RF-05, RF-07 | Must | Heredoc é obrigatório no subject |
| RF-03, RF-06 | Should | Divergentes do bash; mantidos por fidelidade ao as-is |

## Rastreabilidade de Código

| Arquivo | Função | Cobertura |
|---------|--------|-----------|
| `src/exec/here_doc.c` | `here_doc_buffer`, `generation_content`, `putting_nl`, `put_nl_in_buf` | 🟢 |
| `src/exec/here_doc_utils.c` | `creat_here_doc`, `here_doc_verification`, `here_doc_write`, `close_here_doc` | 🟢 |
| `src/facade.c` | `exec_pipe_line` (fork do heredoc, linhas 39-52) | 🟢 |
| `src/signals_utils.c` | `signal_here`, `sig_here` | 🟢 |
| `src/signals.c` | `signal_ignore` | 🟢 |
| `src/exec/creat_cmd_table.c` | `char_cat` | 🟢 |
