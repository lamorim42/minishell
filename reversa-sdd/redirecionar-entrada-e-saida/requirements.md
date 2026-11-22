# Redirecionar Entrada e Saída

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash
> Casos "(observado)" foram executados contra o `main.c` legado compilado na scratchpad (readline substituto lendo stdin, stdout com buffer de linha). O repositório não foi alterado.

## Visão Geral
Abre os arquivos de `<`, `>` e `>>` e liga os descritores ao stdin e ao stdout de cada comando, com prioridade sobre os pipes. Os arquivos são abertos no shell antes de qualquer execução, e os `dup2` são feitos dentro de cada filho. O heredoc (`<<`) entra pelo mesmo mecanismo de entrada, e a leitura do conteúdo fica em `ler-heredoc` 🟢.

## Responsabilidades
- Abrir, no shell e antes dos forks, todos os arquivos de redirecionamento da linha 🟢
- Escolher, para cada comando, a entrada e a saída efetivas 🟢
- Fazer `dup2` dos descritores escolhidos no filho e fechar o resto 🟢
- Informar erros de abertura 🟢

## Regras de Negócio

### Abertura (no shell, antes da execução)
- RD-01: `< alvo` → `open(alvo, O_RDONLY)` 🟢
- RD-02: `> alvo` → `open(alvo, O_RDWR | O_CREAT | O_TRUNC, 0644)` 🟢
- RD-03: `>> alvo` → `open(alvo, O_RDWR | O_CREAT | O_APPEND, 0644)`: `echo a > f1`, `echo b >> f1` → `a`, `b` 🟢 (observado)
- RR-01: **todos** os arquivos de saída da linha são criados ou truncados antes da execução, inclusive os que não serão usados: `echo c > f2 > f3` → `f2` vazio, `f3` com `c` 🟢 (observado), igual ao bash
- O alvo é sempre um `WORD` literal (sem `$`), por causa da gramática (BUG-13) 🟢
- RR-06 / BUG-01: erro ao abrir arquivo de **saída** → `perror(alvo)` e o shell **cai**: `echo x > o3 > /nonexistent/x` → `/nonexistent/x: No such file or directory` seguido de *segfault* (código 139) 🟢 (observado)

### Escolha da entrada (por comando)
- RD-04: se o nó **imediatamente anterior** ao comando é `PIPE`, `REDI` ou `HERE`, o stdin vem dele 🟢
- RD-05: depois, os nós **seguintes** até o próximo `PIPE` são percorridos; o último `REDI`/`HERE` vence: `cat < in1` → conteúdo de `in1` 🟢 (observado)
- RN-43: redirecionamento de entrada vence o pipe: `echo z | cat < in1` → `conteudo` 🟢 (observado), igual ao bash
- RR-03: `< in1 cat` funciona porque o `<` está **imediatamente** antes do comando 🟢 (observado)
- BUG-09 / RR-05: `< inexistente cmd` não dá erro; o comando roda com o stdin original e `$?=0` 🟢 (observado). ⚠️ O bash informa o erro e não executa
- `cmd < inexistente` → `inexistente: No such file or directory` em stderr, o comando não executa, `$?=1` 🟢 (observado)
- RR-04: `ls | > o1 wc -l`: o nó anterior a `wc` é `REDO`, que não é `PIPE` nem entrada, então `wc` **não** lê o pipe e fica com o stdin original (o terminal, numa sessão interativa: espera digitação até Ctrl-D); a saída vai para o terminal e `o1` fica vazio, mas é criado 🟢 (código: `src/fd_handler.c:9-12`; observado com stdin já em EOF: `wc` imprime `0`). [Revisão: corrigida a afirmação anterior de que `wc` lia `o1`.] ⚠️ No bash, `wc` lê o pipe e escreve em `o1`

### Escolha da saída (por comando)
- RD-06: os nós **seguintes** até o próximo `PIPE` são percorridos; o último `REDO`/`REDA` vence 🟢
- RN-43: redirecionamento de saída vence o pipe: `echo a > f5 | cat` → `cat` não recebe nada 🟢 (observado), igual ao bash
- BUG-08 / RR-02: redirecionamento de saída **antes** do comando é ignorado: `> f4 echo d` → `d` no terminal, `f4` vazio (mas criado) 🟢 (observado). ⚠️ No bash, `d` vai para `f4`
- `cat < in1 > o2` → `o2` com o conteúdo de `in1` 🟢 (observado)

### Outros
- Não há suporte a `2>`, `&>`, `<>`, `>|` nem a descritores numerados 🟢
- ✅ L-07 (decidida em 2026-11-22, [Revisão]): **documentar o comportamento atual**. As ⚠️ DIVERGÊNCIAS desta unit são o comportamento especificado, não defeitos a corrigir. (Pendência original: BUG-01 (crash), BUG-08, BUG-09 e RR-04)

## Requisitos Funcionais

| ID | Requisito | Prioridade | Critério de Aceite |
|----|-----------|-----------|-------------------|
| RF-01 | Abrir `<`, `>` e `>>` com os modos de RD-01 a RD-03 | Must | `echo a > f; echo b >> f; cat f` → `a` `b` (linhas separadas) |
| RF-02 | Criar ou truncar todos os arquivos de saída antes da execução | Must | `echo c > f2 > f3` → `f2` vazio |
| RF-03 | Usar o último redirecionamento de cada direção | Must | `echo c > f2 > f3` → `f3` = `c` |
| RF-04 | Dar prioridade ao redirecionamento sobre o pipe | Must | `echo z \| cat < in1` → conteúdo de `in1` |
| RF-05 | Informar arquivo de entrada inexistente e não executar o comando | Must | `cat < nope` → mensagem, `$?=1` |
| RF-06 | Informar erro de abertura de arquivo de saída | Must | `echo x > /nonexistent/x` → `perror` (as-is: o shell cai) |

## Requisitos Não Funcionais

| Tipo | Requisito inferido | Evidência no código | Confiança |
|------|--------------------|---------------------|-----------|
| Disponibilidade | Erro em arquivo de saída derruba o shell (*use-after-free* e *double free*) | `src/fd_handler_utils.c:39-43` | 🟢 (observado) |
| Recursos | Os fds de `REDI` não são fechados no pai depois da linha e são herdados pelos filhos seguintes: depois de 3 `cat < in1`, `ls /proc/self/fd` mostra 3 fds extras | `src/fd_handler.c:60-83`, `src/exec/exec_list.c:27` | 🟢 (observado na revisão de 2026-11-22) |
| Segurança | Arquivos criados com modo `0644` (sujeito ao `umask`) | `src/fd_handler_utils.c:33-37` | 🟢 |

## Critérios de Aceitação

```gherkin
Funcionalidade: Redirecionamentos

  Cenário: Truncar e acrescentar
    Quando o usuário digita "echo a > f1"
    E depois digita "echo b >> f1"
    Então o arquivo "f1" contém "a" e "b"

  Cenário: Último redirecionamento vence
    Quando o usuário digita "echo c > f2 > f3"
    Então "f2" existe e está vazio
    E "f3" contém "c"

  Cenário: Entrada de arquivo
    Dado o arquivo "in1" com "conteudo"
    Quando o usuário digita "cat < in1"
    Então a saída é "conteudo"

  Cenário: Redirecionamento vence o pipe
    Quando o usuário digita "echo z | cat < in1"
    Então a saída é "conteudo"

  Cenário: Arquivo de entrada inexistente depois do comando
    Quando o usuário digita "cat < nope"
    Então "nope: No such file or directory" é escrito em stderr
    E $? é 1

  @divergente
  Cenário: Saída antes do comando é ignorada (BUG-08)
    Quando o usuário digita "> f4 echo d"
    Então "d" aparece no terminal
    E "f4" fica vazio

  @divergente
  Cenário: Entrada inexistente antes do comando (BUG-09)
    Quando o usuário digita "< nope cat"
    Então nenhuma mensagem é exibida

  @divergente @crash
  Cenário: Falha ao abrir arquivo de saída (BUG-01)
    Quando o usuário digita "echo x > /nonexistent/x"
    Então "/nonexistent/x: No such file or directory" é escrito em stderr
    E o shell termina com falha de segmentação
```

## Prioridade (MoSCoW)

| Requisito | MoSCoW | Justificativa |
|-----------|--------|---------------|
| RF-01 a RF-05 | Must | Redirecionamentos são parte obrigatória do subject |
| RF-06 | Must | A mensagem é necessária; o crash não deve ser reproduzido sem decisão |

## Rastreabilidade de Código

| Arquivo | Função | Cobertura |
|---------|--------|-----------|
| `src/fd_handler_utils.c` | `open_fds`, `check_valid_fd`, `close_std_fd` | 🟢 |
| `src/fd_handler.c` | `find_input`, `find_output`, `close_fds`, `is_redirect_output`, `is_redirect_input` | 🟢 |
| `src/exec/exec_path.c` | `make_redirections` | 🟢 |
| `src/exec/init_fork.c` | fechamento de `REDO`/`REDA` no pai | 🟢 |
| `src/main.c` | `file_exists` | 🟢 |
