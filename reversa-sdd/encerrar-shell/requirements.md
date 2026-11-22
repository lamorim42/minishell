# Encerrar Shell

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash
> Casos "(observado)" foram executados contra o `main.c` legado compilado na scratchpad (readline substituto lendo stdin); cada caso foi precedido por um comando com status 2. O repositório não foi alterado.

## Visão Geral
Implementa o builtin `exit [n]`: libera a linha e a tabela e termina o processo do shell com o código pedido. O encerramento por Ctrl-D (EOF no prompt) está em `iniciar-sessao-e-ler-comando` (RN-02) 🟢.

## Responsabilidades
- Terminar o shell com o código numérico informado 🟢
- Validar o argumento numérico 🟢
- Rejeitar mais de um argumento sem encerrar 🟢
- Liberar a memória antes de sair 🟢

## Regras de Negócio
- XT-01: `exit` sem argumento → código **0** 🟢 (observado). ⚠️ O bash usa o último `$?` (aqui seria 2)
- XT-02: `exit n` → código `n & 0xFF` (via `ft_atoi` e `exit`): `exit 7` → 7; `exit -1` → 255; `exit 256` → 0; `exit +5` → 5 🟢 (observado), igual ao bash
- XT-03: um sinal `+` ou `-` inicial é aceito 🟢
- XT-04: argumento não numérico → `<arg> is not a number!` em **stdout** e código **2**: `exit abc`, `exit 5a` 🟢 (observado). ⚠️ O bash escreve `exit: abc: numeric argument required` em stderr (o código 2 coincide)
- XT-05: `exit -` → a mensagem `- is not a number!` aparece, mas o código é **0** 🟢 (observado). ⚠️ O bash dá 2
- XT-06: `exit ""` → código 0, sem mensagem 🟢 (observado). ⚠️ O bash dá 2 com mensagem
- XT-07: valores fora do intervalo de `int` passam pelo `ft_atoi` sem verificação de estouro: `exit 9999999999` → 255 🟢 (observado; coincide com o bash por acaso)
- XT-08: dois ou mais argumentos com o primeiro numérico → `exit: too many arguments` em stderr, `$?=1` e o shell **continua**: `exit 1 2` 🟢 (observado), igual ao bash
- BUG-21 (novo): se o primeiro argumento é **exatamente o número 2**, o laço confunde o valor com o código de erro e encerra: `exit 2 3` → sai com 2 em vez de `too many arguments` 🟢 (observado). Causa: `is_char_number(...) == 2` é usado como teste de "não é número" (`src/builtins/exit.c:51`)
- XT-09: primeiro argumento não numérico com outros depois → mensagem e saída com 2: `exit abc 3` 🟢 (observado), igual ao bash
- XT-10: o builtin não escreve `exit` ao sair 🟢 (observado). ⚠️ O bash interativo escreve `exit` em stderr
- XT-11: `exit` só encerra o shell quando é o último nó da linha; `exit 3 | echo x` roda em filho e o shell continua, mas `ls | exit 3` encerra (BUG-02, ver `executar-pipeline`) 🟢 (observado)
- ✅ L-07 (decidida em 2026-11-22, [Revisão]): **documentar o comportamento atual**. As ⚠️ DIVERGÊNCIAS desta unit são o comportamento especificado, não defeitos a corrigir. (Pendência original: XT-01, XT-04 a XT-06, XT-10 e BUG-21)

## Requisitos Funcionais

| ID | Requisito | Prioridade | Critério de Aceite |
|----|-----------|-----------|-------------------|
| RF-01 | `exit` termina o shell | Must | O processo termina; o código é 0 (as-is) |
| RF-02 | `exit n` termina com `n & 0xFF` | Must | `exit 256` → 0; `exit -1` → 255 |
| RF-03 | Argumento não numérico → mensagem e código 2 | Must | `exit abc` → `abc is not a number!`, código 2 |
| RF-04 | Mais de um argumento → `too many arguments`, `$?=1`, sem encerrar | Must | `exit 1 2`; `echo $?` → `1` |
| RF-05 | Liberar linha e tabela antes de sair | Should | Valgrind sem vazamento ao sair por `exit` |

## Requisitos Não Funcionais

| Tipo | Requisito inferido | Evidência no código | Confiança |
|------|--------------------|---------------------|-----------|
| Memória | `free_line` e `free_table` antes do `exit` | `src/builtins/exit.c:56-58` | 🟢 |
| Robustez | `ft_atoi` sem verificação de estouro | `src/builtins/exit.c:36` | 🟢 |

## Critérios de Aceitação

```gherkin
Funcionalidade: exit

  Cenário: Código explícito
    Quando o usuário digita "exit 7"
    Então o shell termina com código 7

  Cenário: Código módulo 256
    Quando o usuário digita "exit 256"
    Então o shell termina com código 0

  Cenário: Argumento não numérico
    Quando o usuário digita "exit abc"
    Então "abc is not a number!" é escrito em stdout
    E o shell termina com código 2

  Cenário: Argumentos demais
    Quando o usuário digita "exit 1 2"
    Então "exit: too many arguments" é escrito em stderr
    E o shell continua
    E $? é 1

  @divergente
  Cenário: exit sem argumento ignora o último status (XT-01)
    Dado que o último comando terminou com status 2
    Quando o usuário digita "exit"
    Então o shell termina com código 0

  @divergente
  Cenário: Primeiro argumento igual a 2 (BUG-21)
    Quando o usuário digita "exit 2 3"
    Então o shell termina com código 2

  @divergente
  Cenário: Sinal sozinho (XT-05)
    Quando o usuário digita "exit -"
    Então "- is not a number!" é escrito em stdout
    E o shell termina com código 0
```

## Prioridade (MoSCoW)

| Requisito | MoSCoW | Justificativa |
|-----------|--------|---------------|
| RF-01 a RF-04 | Must | `exit` é obrigatório no subject |
| RF-05 | Should | Limpeza; exigida pela avaliação (sem vazamentos) |

## Rastreabilidade de Código

| Arquivo | Função | Cobertura |
|---------|--------|-----------|
| `src/builtins/exit.c` | `exit_builtin`, `is_char_number` | 🟢 |
| `src/free.c` | `free_line`, `error_msg` | 🟢 |
| `src/hash_table/free_hash_table.c` | `free_table` | 🟢 |
| `src/main.c` | `control_d` (encerramento por EOF, ver `iniciar-sessao-e-ler-comando`) | 🟢 |
