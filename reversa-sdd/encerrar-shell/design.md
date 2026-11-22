# Encerrar Shell: Design Técnico

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Interface

| Símbolo | Assinatura | Retorno | Observação |
|---------|-----------|---------|------------|
| `exit_builtin` | `(t_pipe_list *node, t_hash_table **table)` | `void`; não retorna, exceto em `too many arguments` | 🟢 |
| `is_char_number` | `(char *str, int *status_code)` | `int` = `*status_code` | Grava 2 se não for número; senão `ft_atoi(str)` 🟢 |

## Fluxo Principal
`src/builtins/exit.c:40-59` 🟢:
```
status = 0
i = 1
enquanto args[i]:
   se i > 1:
      "exit: too many arguments" em stderr
      g_minishell.line->status_code = 1
      retorna                                  # o shell continua
   se is_char_number(args[1], &status) == 2: pare
   i++
free_line(g_minishell.line); free_table(table)
exit(status)                                   # o kernel aplica & 0xFF
```

### `is_char_number` (`src/builtins/exit.c:17-37`)
```
i = 0
enquanto str[i]:
   se i == 0 e str[0] em "+-": i++
   se str[i] não é dígito:                     # inclui '\0' depois de um sinal sozinho
      printf("%s is not a number!\n", str); status = 2; pare
   i++
se str[i] == '\0': status = ft_atoi(str)       # também roda depois do "pare" se o erro foi no fim
retorna status
```

Consequências do algoritmo 🟢:
- **BUG-21:** o retorno é o próprio código; um número válido igual a 2 é confundido com o erro e faz o laço parar antes de contar os argumentos
- **XT-05:** em `exit -`, o erro é detectado em `str[1] == '\0'`; o `pare` deixa `i = 1`, então `str[i] == '\0'` é verdadeiro e `status = ft_atoi("-") = 0`
- **XT-06:** em `exit ""`, o laço não roda e `status = ft_atoi("") = 0`

## Fluxos Alternativos
- **Em filho** (`exit 3 | echo x`, `exit 3 > f`): `exit` termina só o filho; o pai segue (BUG-10 não se aplica, porque o filho sai pelo próprio `exit`) 🟢
- **Último nó de pipeline** (`ls | exit 3`): roda no pai e encerra o shell (BUG-02) 🟢
- **`too many arguments`:** grava `status_code = 1` e retorna; `update_status_code` publica `$?=1` 🟢

## Dependências
- **executar-comando-simples / executar-pipeline:** decidem se o builtin roda no shell ou num filho
- **gerenciar-variaveis-de-ambiente:** `free_table`
- **Libft:** `ft_isdigit`, `ft_atoi`

## Decisões de Design Identificadas

| Decisão | Evidência no código | Confiança |
|---------|---------------------|-----------|
| Mensagem de erro numérico em stdout (`printf`) e a de argumentos em stderr (`error_msg`) | `src/builtins/exit.c:28,47` | 🟢 |
| O código de erro (2) e o valor convertido compartilham a mesma variável | `src/builtins/exit.c:29,36,51` | 🟢 |
| `exit` sem argumento usa 0, não `$?` | `src/builtins/exit.c:44` | 🟢 |

## Estado Interno
Nenhum além de `status_code` local 🟢.

## Observabilidade
`<arg> is not a number!` (stdout) e `exit: too many arguments` (stderr) 🟢.

## Riscos e Lacunas
- 🟢 BUG-21: `exit 2 <qualquer coisa>` encerra
- 🟢 XT-05/XT-06: `exit -` e `exit ""` saem com 0
- 🟢 XT-01: `exit` sem argumento ignora `$?`
- ✅ L-07 (decidida em 2026-11-22): documentar o as-is. Nota para uma eventual reimplementação corretiva, fora do escopo atual: mensagens no formato do bash (`exit: <arg>: numeric argument required`, em stderr) e `exit` escrito ao sair
