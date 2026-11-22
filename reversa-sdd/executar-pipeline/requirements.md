# Executar Pipeline

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash
> Casos "(observado)" foram executados contra o `main.c` legado compilado na scratchpad (readline substituto lendo stdin, stdout com buffer de linha). O repositório não foi alterado.

## Visão Geral
Encadeia dois ou mais comandos com `|`, ligando o stdout de cada um ao stdin do seguinte por um `pipe()`. Os comandos rodam em paralelo e `$?` recebe o status do último. Usa a montagem da lista e a execução de `executar-comando-simples` 🟢.

## Responsabilidades
- Criar um `pipe()` para cada nó `PIPE`, no pai e antes de qualquer `fork` 🟢
- Ligar, em cada filho, a ponta de leitura do pipe anterior ao stdin e a ponta de escrita do pipe seguinte ao stdout 🟢
- Fechar no pai as pontas de cada pipe assim que o comando seguinte foi criado 🟢
- Esperar todos os filhos e gravar o status do último em `$?` 🟢

## Regras de Negócio
- RN-34: os comandos de um pipeline rodam em **paralelo**: `sleep 1 | sleep 1 | echo x` termina em ~1 s 🟢 (observado)
- RN-34: `$?` é o status do **último** comando: `ls /nope | true` → 0; `true | ls /nope` → 2 🟢 (observado)
- PL-01: dados fluem por pipelines de qualquer tamanho: `echo abc | cat | cat | wc -c` → `4` 🟢 (observado)
- PL-02: um leitor que termina cedo não trava o pipeline: `yes | head -1` → `y`, `$?=0` 🟢 (observado)
- RN-36 / BUG-02: builtin **no último nó** roda no **shell**, mesmo depois de um pipe:
  - `ls | exit 3` encerra o shell com código 3 🟢 (observado). ⚠️ No bash, cada parte do pipeline roda em subshell e o shell continua
  - `echo a | cd /tmp` muda o diretório do shell 🟢 (observado). ⚠️ No bash não muda
  - o builtin final não lê o pipe; o pipe anterior é fechado sem leitor 🟢
- PL-03: com builtin no último nó, `$?` fica com o status do **último filho esperado**, não do builtin: `ls /nope | echo x` → `x` e `$?=2` 🟢 (observado). ⚠️ O bash dá 0
- PL-04: builtin fora do último nó roda em filho e não afeta o shell: `exit 3 | echo depois` → `depois`, o shell continua; `export PX=1 | cat` não cria `PX` 🟢 (observado), igual ao bash
- RN-12: `|` no fim da linha é aceito; o comando escreve num pipe sem leitor e nada aparece: `ls |` → `$?=0` 🟢 (observado)
- PL-05: comando inexistente no meio não interrompe os outros: `echo a | naoexiste | wc -l` → `naoexiste: command not found` e `0`; `$?=0` 🟢 (observado), igual ao bash
- PL-06: o último nó vazio ou ignorado (BUG-20) deixa o comando anterior sem leitor válido: `echo a | cat -e | ""` → `cat: -: Bad file descriptor`, `$?=1` 🟢 (observado)
- RN-43: redirecionamento explícito tem prioridade sobre o pipe (detalhado em `redirecionar-entrada-e-saida`) 🟢
- ✅ L-07 (decidida em 2026-11-22, [Revisão]): **documentar o comportamento atual**. As ⚠️ DIVERGÊNCIAS desta unit são o comportamento especificado, não defeitos a corrigir. (Pendência original: BUG-02 (builtin final no shell) e PL-03)

## Requisitos Funcionais

| ID | Requisito | Prioridade | Critério de Aceite |
|----|-----------|-----------|-------------------|
| RF-01 | Criar um pipe por `\|` antes de criar os processos | Must | `ls \| wc -l` imprime a contagem |
| RF-02 | Ligar stdout do comando *i* ao stdin do comando *i+1* | Must | `echo abc \| cat \| cat \| wc -c` → `4` |
| RF-03 | Executar os comandos em paralelo | Must | `sleep 1 \| sleep 1` termina em ~1 s |
| RF-04 | Fechar as pontas dos pipes no pai | Must | `yes \| head -1` termina |
| RF-05 | Gravar em `$?` o status do último filho | Must | `true \| ls /nope`; `echo $?` → `2` |
| RF-06 | Rodar o builtin do último nó no shell (as-is, BUG-02) | Should | `ls \| exit 3` encerra o shell com 3 |

## Requisitos Não Funcionais

| Tipo | Requisito inferido | Evidência no código | Confiança |
|------|--------------------|---------------------|-----------|
| Recursos | Todos os pipes da linha são criados de uma vez, antes dos forks; o limite de fds do processo limita o tamanho do pipeline | `src/fd_handler_utils.c:15-28` | 🟢 |
| Concorrência | Os filhos são esperados na ordem de criação (`waitpid` com `WUNTRACED`) | `src/exec/exec_list.c:42-52` | 🟢 |

## Critérios de Aceitação

```gherkin
Funcionalidade: Pipeline

  Cenário: Dois comandos
    Quando o usuário digita "echo abc | wc -c"
    Então a saída é "4"

  Cenário: Pipeline longo
    Quando o usuário digita "echo abc | cat | cat | wc -c"
    Então a saída é "4"

  Cenário: Status do último comando
    Quando o usuário digita "true | ls /nope"
    E depois digita "echo $?"
    Então a saída é "2"

  Cenário: Execução paralela
    Quando o usuário digita "sleep 1 | sleep 1 | echo x"
    Então "x" aparece em cerca de 1 segundo

  Cenário: Builtin no meio do pipeline
    Quando o usuário digita "exit 3 | echo depois"
    Então a saída é "depois"
    E o shell continua aberto

  @divergente
  Cenário: exit no último nó encerra o shell (BUG-02)
    Quando o usuário digita "ls | exit 3"
    Então o shell termina com código 3

  @divergente
  Cenário: Status com builtin final (PL-03)
    Quando o usuário digita "ls /nope | echo x"
    E depois digita "echo $?"
    Então a saída é "2"

  @divergente
  Cenário: Pipe no fim da linha
    Quando o usuário digita "ls |"
    Então nada é exibido
```

## Prioridade (MoSCoW)

| Requisito | MoSCoW | Justificativa |
|-----------|--------|---------------|
| RF-01 a RF-05 | Must | Pipes são parte obrigatória do subject |
| RF-06 | Should | Comportamento divergente, mantido só por fidelidade ao as-is |

## Rastreabilidade de Código

| Arquivo | Função | Cobertura |
|---------|--------|-----------|
| `src/fd_handler_utils.c` | `open_fds` (ramo `PIPE`) | 🟢 |
| `src/fd_handler.c` | `find_input`, `find_output`, `close_fds` (ramos `PIPE`) | 🟢 |
| `src/exec/init_fork.c` | `init_fork` (fechamento do pipe anterior no pai) | 🟢 |
| `src/exec/exec_list.c` | `exec_list`, `wait_child`, `update_status_code` | 🟢 |
| `src/exec/creat_cmd_table.c` | `get_arg_size` (nó `PIPE` de tamanho 1) | 🟢 |
