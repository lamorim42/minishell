# Navegar Diretórios: Design Técnico

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Interface

| Símbolo | Assinatura | Retorno | Observação |
|---------|-----------|---------|------------|
| `cd_builtin` | `(t_pipe_list *node, t_hash_table **table)` | `void` | Usa só `args[1]` 🟢 |
| `cd_expand` (static) | `(t_hash_table **table, char *old_pwd, char *expand)` | `void` | Hoje sempre chamado com `"HOME"` 🟢 |
| `update_var_env` | `(t_hash_table **table, char *key, char *val)` | `void` | `table_delete` (se existir) + `hash_insert`; também usado por `export` 🟢 |
| `pwd_builtin` | `(t_hash_table **table)` | `void` | `printf("%s\n", PWD)` se existir 🟢 |

Erros vão para `g_minishell.line->status_code` (lido por `update_status_code`) 🟢.

## Fluxo Principal

### `cd` (`src/builtins/cd.c:19-40`)
```
getcwd(buffer, 500); old = strdup(buffer)
se args[1] == NULL ou args[1] == "~":
   cd_expand(table, old, "HOME")
senão se chdir(args[1]) == -1:
   status_code = 1; perror(args[1])
senão:
   getcwd(buffer, 500)
   update_var_env("PWD", buffer)
   update_var_env("OLDPWD", old)
free(old)
```

### `cd_expand` (`src/builtins/cd.c:53-72`)
```
v = search_item(table, "HOME")
se v:
   se chdir(v) == -1: status_code = 1; perror(v); retorna
   update_var_env("PWD", v)            # valor literal, sem getcwd
   update_var_env("OLDPWD", old)
senão: "cd: HOME not set" em stderr     # status_code não é alterado
```

### `pwd` (`src/builtins/pwd.c:15-22`)
```
v = search_item(table, "PWD")
se v: printf("%s\n", v)
```

## Fluxos Alternativos
- **`cd` em filho** (com redirecionamento ou não final num pipeline): roda, mas o `chdir` acontece no filho; o shell não muda e `$?=0` (BUG-10) 🟢
- **`HOME` vazio** (`HOME=` herdado): `chdir("")` falha → `perror("")` → `: No such file or directory`... na prática a saída observada é `No such file or directory`, e `$?=1` 🟢 (observado com `cd ""`)
- **`getcwd` falho** (caminho maior que 500 bytes ou diretório removido): `buffer` não é atualizado; `old`/`PWD` ficam com lixo ou com o valor anterior 🟡

## Dependências
- **gerenciar-variaveis-de-ambiente:** `search_item`, `update_var_env`, `table_delete`, `hash_insert`
- **executar-comando-simples:** `exec_builtins`; efeito só como último nó
- **POSIX:** `chdir`, `getcwd`

## Decisões de Design Identificadas

| Decisão | Evidência no código | Confiança |
|---------|---------------------|-----------|
| `pwd` lê `PWD` da tabela em vez de chamar `getcwd` | `src/builtins/pwd.c:19` | 🟢 |
| `PWD` com `getcwd` no caminho explícito e com o valor literal de `HOME` no `cd` sem argumento | `src/builtins/cd.c:32-33,67` | 🟢 |
| Buffer de 500 bytes na pilha | `src/builtins/cd.c:20` | 🟢 |
| `cd_expand` foi escrito genérico (`expand`), mas só é usado com `HOME` | `src/builtins/cd.c:24,53` | 🟡 |

## Estado Interno
Diretório de trabalho do processo (kernel) e as chaves `PWD`/`OLDPWD` da tabela. Os dois só ficam coerentes se nada alterar `PWD` por fora (`export PWD=...`, `unset PWD`) 🟢.

## Observabilidade
`perror(<dir>)`, `cd: HOME not set` 🟢.

## Riscos e Lacunas
- 🟢 BUG-12: `HOME not set` não grava `$?=1`
- 🟢 `pwd` incoerente com o diretório real (PW-02 a PW-04)
- 🟡 Caminhos maiores que 500 bytes
- ✅ L-07 (decidida em 2026-11-22): documentar o as-is. Nota para uma eventual reimplementação corretiva, fora do escopo atual: `cd -`, `~/x`, `too many arguments` e o prefixo `cd: ` nas mensagens fazem parte do alvo?
