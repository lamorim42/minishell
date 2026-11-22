# Navegar Diretórios

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash
> Casos "(observado)" foram executados contra o `main.c` legado compilado na scratchpad. Ambiente de teste: `HOME=/tmp PWD=/start PATH=/usr/bin:/bin`. O repositório não foi alterado.

## Visão Geral
Implementa os builtins `cd` (mudar o diretório de trabalho do shell e atualizar `PWD`/`OLDPWD` na tabela) e `pwd` (mostrar o diretório). O `pwd` mostra a variável `PWD` da tabela, não o diretório real do processo 🟢.

## Responsabilidades
- `cd <dir>`: `chdir` e atualização de `PWD` e `OLDPWD` 🟢
- `cd` e `cd ~`: ir para `$HOME` 🟢
- Informar erros de `chdir` e `HOME` ausente 🟢
- `pwd`: imprimir o valor de `PWD` 🟢

## Regras de Negócio

### `cd`
- CD-01: `cd <dir>` → `chdir(dir)`; em sucesso, `PWD` = `getcwd()` (caminho resolvido) e `OLDPWD` = diretório anterior (`getcwd()` antes do `chdir`): `cd /usr/../etc` → `PWD=/etc` 🟢 (observado)
- CD-02: `cd` sem argumento ou `cd ~` (exatamente) → `chdir($HOME)`; `PWD` recebe o **valor literal** de `HOME`, sem resolver: `HOME=/usr/../tmp` → `PWD=/usr/../tmp` 🟢 (observado). ⚠️ O bash grava o caminho lógico normalizado
- CD-03: só o primeiro argumento é usado; os outros são ignorados: `cd /usr bin` → `/usr` 🟢 (observado). ⚠️ O bash recente reclama de `too many arguments`
- CD-04: `~` só é expandido sozinho: `cd ~/x` → `~/x: No such file or directory` 🟢 (observado). ⚠️ O bash expande o til
- CD-05: `cd -` não é suportado → `-: No such file or directory`, `$?=1` 🟢 (observado). ⚠️ O bash vai para `$OLDPWD`
- CD-06: falha de `chdir` → `perror(<dir>)` e `$?=1`: `cd /nope` → `/nope: No such file or directory` 🟢 (observado). ⚠️ O bash prefixa com `cd: `
- CD-07 / BUG-12: `HOME` ausente → `cd: HOME not set` em stderr e **`$?=0`** 🟢 (observado). ⚠️ O bash grava 1
- CD-08: `cd ""` → `chdir("")` falha → `No such file or directory` (sem nome) e `$?=1` 🟢 (observado). ⚠️ O bash não faz nada e retorna 0
- CD-09: `cd` só muda o diretório do shell quando é o último nó da linha (RE-01): `cd /tmp > /dev/null` não tem efeito 🟢 (observado em `executar-comando-simples`)
- CD-10: o diretório anterior é lido num buffer fixo de 500 bytes; caminhos maiores deixam `OLDPWD` indefinido 🟡

### `pwd`
- PW-01: `pwd` imprime o valor de `PWD` **da tabela**, seguido de `\n` 🟢
- PW-02: o valor herdado do ambiente é usado sem conferência: com `PWD=/start` herdado, `pwd` → `/start` 🟢 (observado). ⚠️ O bash confere se `PWD` aponta para o diretório atual
- PW-03: `export PWD=/fake` → `pwd` imprime `/fake` (enquanto `/bin/pwd` imprime o real) 🟢 (observado). ⚠️ O bash usa o diretório real
- PW-04: sem `PWD` na tabela (`unset PWD` ou ambiente vazio), `pwd` **não imprime nada** 🟢 (observado). ⚠️ O bash imprime o diretório real
- PW-05: argumentos são ignorados 🟢
- ✅ L-07 (decidida em 2026-11-22, [Revisão]): **documentar o comportamento atual**. As ⚠️ DIVERGÊNCIAS desta unit são o comportamento especificado, não defeitos a corrigir. (Pendência original: CD-02 a CD-08 e PW-02 a PW-04)

## Requisitos Funcionais

| ID | Requisito | Prioridade | Critério de Aceite |
|----|-----------|-----------|-------------------|
| RF-01 | `cd <dir>` muda o diretório e atualiza `PWD`/`OLDPWD` | Must | `cd /usr`; `pwd` → `/usr`; `echo $OLDPWD` → diretório anterior |
| RF-02 | `cd` e `cd ~` vão para `$HOME` | Must | `cd`; `pwd` → valor de `HOME` |
| RF-03 | `chdir` falho → mensagem e `$?=1` | Must | `cd /nope` → mensagem; `echo $?` → `1` |
| RF-04 | `HOME` ausente → `cd: HOME not set` | Should | `unset HOME`; `cd` → mensagem |
| RF-05 | `pwd` imprime o diretório atual | Must | Depois de `cd /etc`, `pwd` → `/etc` |

## Requisitos Não Funcionais

| Tipo | Requisito inferido | Evidência no código | Confiança |
|------|--------------------|---------------------|-----------|
| Robustez | Buffer fixo de 500 bytes para `getcwd`; retorno não verificado | `src/builtins/cd.c:22-23,32` | 🟢 |
| Consistência | `pwd` depende de `PWD` estar coerente; nada sincroniza a tabela com o diretório real | `src/builtins/pwd.c:15-22` | 🟢 |

## Critérios de Aceitação

```gherkin
Funcionalidade: cd e pwd

  Cenário: Mudar de diretório
    Quando o usuário digita "cd /usr"
    E depois digita "pwd"
    Então a saída é "/usr"

  Cenário: Caminho com ".." é resolvido
    Quando o usuário digita "cd /usr/../etc"
    E depois digita "pwd"
    Então a saída é "/etc"

  Cenário: cd sem argumento
    Dado HOME=/tmp
    Quando o usuário digita "cd"
    E depois digita "pwd"
    Então a saída é "/tmp"

  Cenário: Diretório inexistente
    Quando o usuário digita "cd /nope"
    Então "/nope: No such file or directory" é escrito em stderr
    E $? é 1

  @divergente
  Cenário: HOME ausente não altera $? (BUG-12)
    Dado que HOME não existe
    Quando o usuário digita "cd"
    Então "cd: HOME not set" é escrito em stderr
    E $? é 0

  @divergente
  Cenário: cd - não é suportado
    Quando o usuário digita "cd -"
    Então "-: No such file or directory" é escrito em stderr

  @divergente
  Cenário: pwd usa a variável PWD
    Quando o usuário digita "export PWD=/fake"
    E depois digita "pwd"
    Então a saída é "/fake"

  @divergente
  Cenário: pwd sem PWD
    Quando o usuário digita "unset PWD"
    E depois digita "pwd"
    Então nada é exibido
```

## Prioridade (MoSCoW)

| Requisito | MoSCoW | Justificativa |
|-----------|--------|---------------|
| RF-01, RF-02, RF-03, RF-05 | Must | `cd` (caminho relativo ou absoluto) e `pwd` são obrigatórios no subject |
| RF-04 | Should | Mensagem auxiliar |

## Rastreabilidade de Código

| Arquivo | Função | Cobertura |
|---------|--------|-----------|
| `src/builtins/cd.c` | `cd_builtin`, `cd_expand`, `update_var_env` | 🟢 |
| `src/builtins/pwd.c` | `pwd_builtin` | 🟢 |
| `src/free.c` | `error_msg` | 🟢 |
