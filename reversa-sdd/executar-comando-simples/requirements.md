# Executar Comando Simples

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash
> Casos "(observado)" foram executados contra o `main.c` legado compilado na scratchpad, com um `readline` substituto que lê linhas do stdin e stdout com buffer de linha (como num terminal). O repositório não foi alterado.

## Visão Geral
Transforma os tokens limpos em uma **lista de nós** (comandos e operadores), resolve o binário de cada comando (builtin, caminho literal ou busca no `PATH`), executa e grava o status em `$?`. Esta unit cobre a linha com **um** comando; a combinação com `|` fica em `executar-pipeline` e a abertura de arquivos em `redirecionar-entrada-e-saida` 🟢.

## Responsabilidades
- Agrupar os tokens em nós `t_pipe_list` (lista duplamente encadeada) 🟢
- Classificar cada nó de comando: builtin, caminho literal ou busca no `PATH` 🟢
- Executar builtins no próprio shell quando o comando é o último nó da linha 🟢
- Executar os demais comandos em um processo filho com `execve` 🟢
- Esperar os filhos e converter o status em `$?` 🟢
- Informar `command not found` e erros do `execve` 🟢

## Regras de Negócio

### Montagem da lista
- ML-01: uma sequência de tokens `WORD`/`VAR` forma **um** nó de comando; `args` = tokens limpos (`ctks`) 🟢
- ML-02: cada operador de redirecionamento forma um nó de 2 posições: `args[0]` = lexema (`"REDO"`, `"REDA"`, `"REDI"`, `"HERE"`) e `args[1]` = alvo 🟢
- ML-03: `|` forma um nó de 1 posição, `args[0]` = `"PIPE"` 🟢
- ML-04: palavras **depois** de um redirecionamento formam um **novo** nó de comando: `echo a > f b` → `[echo a] [REDO f] [b]`; `b` é executado como comando (`b: command not found`, `$?=127`) e `f` recebe `a` 🟢 (observado). ⚠️ O bash trata `b` como argumento de `echo`

### Resolução do binário
- RN-30: nome que começa com `/`, `./` ou `../` é usado como caminho literal 🟢. ⚠️ O bash aceita qualquer nome com `/`: `d/s` → `command not found` no minishell 🟢 (observado)
- RN-31 / RE-06: os outros nomes são procurados nos diretórios do `PATH` **da tabela de ambiente**, na ordem, e vence o primeiro que **existe** (`access(F_OK)`), mesmo sem permissão de execução 🟢
- `PATH` removido (`unset PATH`) ou ausente → todo comando sem `/` dá `command not found` 🟢 (observado: `ls` → 127)
- RN-32 / BUG-04: o nome é considerado builtin quando é **prefixo** de `echo`, `cd`, `pwd`, `export`, `unset`, `env` ou `exit`, ou quando tem um deles como prefixo (comparação com `ft_strncmp`). Depois, a execução exige igualdade exata; se não houver, a resposta é `<nome>: command not found` e `$?=127`. Exemplos: `pwdx`, `envsubst`, `e`, `ec`, `ex` 🟢 (observado). ⚠️ O bash executaria o programa externo
- BUG-20 (novo): um comando cujo nome é **prefixo** de `PIPE`, `REDO`, `REDA`, `REDI` ou `HERE` (`P`, `PI`, `R`, `RE`, `H`, `HE`...) ou é **vazio** é tratado como operador e **ignorado em silêncio**: nada executa e `$?` não muda. `$NOPE ls` não executa `ls` 🟢 (observado). Causa: o `ft_strncmp` da Libft retorna 0 quando uma das strings termina antes de `n` (`Libft/ft_strncmp.c`). ⚠️ O bash executaria `P` ou `ls`

### Execução
- RN-36 / RE-01: builtin no **último nó da lista** roda no próprio shell (altera diretório e ambiente) 🟢 (observado: `cd /tmp` seguido de `pwd` → `/tmp`)
- RE-01: se houver qualquer redirecionamento **depois** do builtin, ele deixa de ser o último nó e roda em um filho, sem efeito no shell: `cd /tmp > /dev/null` não muda o diretório 🟢 (observado). ⚠️ No bash o `cd` funciona
- RE-02 / BUG-10: builtin em filho sempre sai com **0**: `cd nope > /dev/null` → mensagem de erro e `$?=0` 🟢 (observado)
- RE-03 / BUG-03: o `execve` recebe o `envp` **original** do processo, não a tabela: depois de `export ZZ=1`, `sh -c 'echo $ZZ'` imprime vazio; depois de `unset HOME`, o filho ainda vê `HOME` 🟢 (observado). ⚠️ No bash os filhos herdam as mudanças
- RE-04: comando não encontrado → `<nome>: command not found` em stderr, `$?=127` 🟢 (observado)
- RE-05 / BUG-16: falha do `execve` (diretório, arquivo sem permissão de execução) → `perror(<nome>)` e `$?=127`: `/etc` → `/etc: Permission denied`; `./d/s` sem `+x` → `./d/s: Permission denied` 🟢 (observado). ⚠️ O bash usa 126
- RE-07: comando relativo sem `./` (`d/s`) só é procurado no `PATH` 🟢 (observado)
- RN-34 (parcial): `$?` recebe o status do último filho esperado. Se nenhum filho rodou (builtin no pai), vale o `status_code` deixado pelo `waitpid` do filho do heredoc, que roda em toda linha e sai com 0, a menos que o builtin o tenha alterado (ex.: `export 1A=2` → 1). Por isso `ls /nope` seguido de `pwd` dá `$?=0` 🟢 (observado)

### `$?`
- `WIFEXITED` → código de saída; `WIFSIGNALED` → `128 + sinal`; os outros casos copiam o valor bruto (`src/exec/exec_list.c:54-69`) 🟢
- ✅ L-07 (decidida em 2026-11-22, [Revisão]): **documentar o comportamento atual**. As ⚠️ DIVERGÊNCIAS desta unit são o comportamento especificado, não defeitos a corrigir. (Pendência original: ML-04, RN-32, BUG-20, RE-01 (redirecionamento depois de builtin), RE-02, RE-03 e RE-05)

## Requisitos Funcionais

| ID | Requisito | Prioridade | Critério de Aceite |
|----|-----------|-----------|-------------------|
| RF-01 | Montar a lista de nós conforme ML-01 a ML-04 | Must | `echo a b > out` → `[echo a b] [REDO out]` |
| RF-02 | Executar programa pelo caminho literal quando o nome começa com `/`, `./` ou `../` | Must | `/bin/echo oi` → `oi` |
| RF-03 | Procurar o programa no `PATH` da tabela | Must | `ls` lista o diretório; depois de `unset PATH`, `ls` → 127 |
| RF-04 | Executar builtins no shell quando são o último nó | Must | `cd /tmp` muda o diretório do shell |
| RF-05 | Executar externos em filho com `execve` e esperar o término | Must | `sleep 1` bloqueia o prompt por 1 s |
| RF-06 | Informar `command not found` com `$?=127` | Must | `naoexiste` → mensagem em stderr; `echo $?` → `127` |
| RF-07 | Informar erro do `execve` com `perror` e `$?=127` (as-is) | Should | `/etc` → `/etc: Permission denied` |
| RF-08 | Gravar o status do último comando em `$?` | Must | `ls /nao-existe`; `echo $?` → `2` |

## Requisitos Não Funcionais

| Tipo | Requisito inferido | Evidência no código | Confiança |
|------|--------------------|---------------------|-----------|
| Isolamento | Externos e builtins não finais rodam em processo filho; o pai só faz `waitpid` | `src/exec/init_fork.c:15-30`, `src/exec/exec_list.c:30-40` | 🟢 |
| Segurança | A busca no `PATH` aceita arquivos sem permissão de execução (`F_OK`), e o erro só aparece no `execve` | `src/exec/path_finder.c:39` | 🟢 |
| Memória | O filho libera a linha e a tabela antes de sair (`free_exit`) | `src/free.c:24-29` | 🟢 |

## Critérios de Aceitação

```gherkin
Funcionalidade: Execução de um comando

  Cenário: Programa encontrado no PATH
    Dado PATH=/usr/bin:/bin
    Quando o usuário digita "ls /"
    Então o conteúdo de "/" é exibido
    E $? é 0

  Cenário: Caminho literal
    Quando o usuário digita "/bin/echo absoluto"
    Então a saída é "absoluto"

  Cenário: Builtin no shell
    Quando o usuário digita "cd /tmp"
    E depois digita "pwd"
    Então a saída é "/tmp"

  Cenário: Comando inexistente
    Quando o usuário digita "naoexiste"
    Então "naoexiste: command not found" é escrito em stderr
    E $? é 127

  Cenário: Status de saída propagado
    Quando o usuário digita "ls /nao-existe"
    E depois digita "echo $?"
    Então a saída é "2"

  @divergente
  Cenário: Palavra depois de redirecionamento vira comando (ML-04)
    Quando o usuário digita "echo a > f b"
    Então "b: command not found" é escrito em stderr
    E o arquivo "f" contém "a"

  @divergente
  Cenário: Nome com prefixo de builtin (BUG-04)
    Quando o usuário digita "pwdx"
    Então "pwdx: command not found" é escrito em stderr

  @divergente
  Cenário: Comando ignorado em silêncio (BUG-20)
    Dado que a variável NOPE não existe
    Quando o usuário digita "$NOPE ls"
    Então nada é exibido
    E $? não muda

  @divergente
  Cenário: Builtin com redirecionamento roda em filho (RE-01)
    Quando o usuário digita "cd /tmp > /dev/null"
    E depois digita "pwd"
    Então o diretório não mudou

  @divergente
  Cenário: export não chega aos filhos (BUG-03)
    Quando o usuário digita "export ZZ=1"
    E depois digita "sh -c 'echo $ZZ'"
    Então a saída é uma linha vazia

  @divergente
  Cenário: execve falha com 127 (BUG-16)
    Quando o usuário digita "/etc"
    Então "/etc: Permission denied" é escrito em stderr
    E $? é 127
```

## Prioridade (MoSCoW)

| Requisito | MoSCoW | Justificativa |
|-----------|--------|---------------|
| RF-01, RF-02, RF-03, RF-05, RF-06, RF-08 | Must | Caminho crítico de toda linha executável |
| RF-04 | Must | Sem ele `cd`, `export`, `unset` e `exit` não têm efeito |
| RF-07 | Should | Mensagem de erro com código divergente (126 no bash) |

## Rastreabilidade de Código

| Arquivo | Função | Cobertura |
|---------|--------|-----------|
| `src/exec/creat_cmd_table.c` | `creat_list_cmd`, `make_args`, `get_arg_size` | 🟢 |
| `src/exec/make_list.c` | `add_back_list`, `new_node` | 🟢 |
| `src/arr_utils.c` | `copy_array` | 🟢 |
| `src/exec/list_generation_bin.c` | `list_generation_bin`, `is_path` | 🟢 |
| `src/exec/path_finder.c` | `path_finder`, `split_path`, `access_path`, `get_path` | 🟢 |
| `src/builtins/builtins_utils.c` | `is_a_builtin`, `exec_builtins` | 🟢 |
| `src/exec/exec_utils.c` | `is_command` | 🟢 |
| `src/exec/exec_list.c` | `exec_list`, `wait_child`, `update_status_code` | 🟢 |
| `src/exec/init_fork.c` | `init_fork`, `population_arr_pid` | 🟢 |
| `src/exec/exec_path.c` | `exec_path`, `make_redirections` | 🟢 |
| `src/facade.c` | `exec_pipe_line`, `alloc_commands` | 🟢 |
| `src/free.c` | `error_msg`, `free_exit`, `free_list` | 🟢 |
| `Libft/ft_strncmp.c` | `ft_strncmp` (semântica de prefixo, causa do BUG-20) | 🟢 |
