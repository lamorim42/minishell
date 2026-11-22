# Executar Comando Simples: Tarefas de Implementação

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Pré-requisitos
- [ ] Units `analisar-linha-de-comando` e `expandir-variaveis` implementadas (`lex` e `ctks`)
- [ ] Tabela de ambiente com `search_item` (unit `gerenciar-variaveis-de-ambiente`)
- [x] Decisão de L-07 sobre a semântica de comparação: reproduzir o `ft_strncmp` por prefixo (as-is) ou usar igualdade exata → **L-07 decidida em 2026-11-22: documentar o as-is**

## Tarefas

- [ ] T-01, Definir `t_pipe_list` com `args`, `union { bin; fd[2]; }`, `next` e `prev`, e as funções `new_node` / `add_back_list`
  - Origem no legado: `includes/minishell.h`, `src/exec/make_list.c`
  - Critério de pronto: lista de 3 nós com `prev` e `next` coerentes
  - Confiança: 🟢

- [ ] T-02, Implementar `get_arg_size`, `make_args` e `creat_list_cmd` (ML-01 a ML-04)
  - Origem no legado: `src/exec/creat_cmd_table.c:18-67`
  - Critério de pronto: `echo a b > out | wc -l < in` → `[echo a b] [REDO out] [PIPE] [wc -l] [REDI in]`; `echo a > f b` → `[echo a] [REDO f] [b]`
  - Confiança: 🟢
  - Nota: juntar as palavras depois do redirecionamento ao comando anterior alinha com o bash; depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-03, Implementar `is_command` e `is_a_builtin`
  - Origem no legado: `src/exec/exec_utils.c:15-22`, `src/builtins/builtins_utils.c:40-51`, `Libft/ft_strncmp.c`
  - Critério de pronto (as-is): `pwdx` e `e` → builtin; `P` e `""` → não comando. Critério de pronto (corrigido): igualdade exata nos dois casos
  - Confiança: 🟢
  - Nota: BUG-04 e BUG-20 vêm da mesma causa; tratar junto com L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-04, Implementar `path_finder` com a busca no `PATH` da tabela e `access(F_OK)`
  - Origem no legado: `src/exec/path_finder.c:15-75`
  - Critério de pronto: `ls` → `/usr/bin/ls` (ou o primeiro diretório que o contenha); `PATH` ausente → `NULL`
  - Confiança: 🟢

- [ ] T-05, Implementar `list_generation_bin` (builtin → `"builtin"`; `/`, `./`, `../` → literal; senão `path_finder`)
  - Origem no legado: `src/exec/list_generation_bin.c:17-42`
  - Critério de pronto: `d/s` → `bin = NULL` (as-is); `./d/s` → `bin = "./d/s"`
  - Confiança: 🟢

- [ ] T-06, Implementar `alloc_commands` (contar nós de comando e alocar `pid[]` zerado)
  - Origem no legado: `src/facade.c:61-76`
  - Critério de pronto: `count_cmds` igual ao número de nós de comando; `pid[count_cmds] == 0`
  - Confiança: 🟢

- [ ] T-07, Implementar `exec_path` no filho: redirecionamentos → `command not found` (127) → builtin (`exit 0`) → `execve(bin, args, envp_original)` → `perror` (127)
  - Origem no legado: `src/exec/exec_path.c:17-39`
  - Critério de pronto: cenários "Comando inexistente" e "execve falha com 127"
  - Confiança: 🟢
  - Nota: passar a tabela como `envp` (BUG-03), sair com o status real do builtin (BUG-10) e usar 126 (BUG-16) dependem de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-08, Implementar `init_fork`: `signals_parent`, `fork`, `close_std_fd` no filho, registro do pid no pai
  - Origem no legado: `src/exec/init_fork.c:17-42`
  - Critério de pronto: `sleep 1` bloqueia o prompt por 1 s; nenhum fd de `std_fd` aberto no filho
  - Confiança: 🟢

- [ ] T-09, Implementar `exec_list` com o builtin no pai quando `next == NULL`, `wait_child` e `update_status_code`
  - Origem no legado: `src/exec/exec_list.c:18-69`
  - Critério de pronto: cenários "Builtin no shell", "Status de saída propagado" e "Builtin com redirecionamento roda em filho" (as-is)
  - Confiança: 🟢

- [ ] T-10, Implementar `error_msg` e `free_exit`
  - Origem no legado: `src/free.c:15-29`
  - Critério de pronto: `naoexiste` → `naoexiste: command not found` em stderr; Valgrind no filho sem vazamento
  - Confiança: 🟢

## Tarefas de Teste

- [ ] TT-01, Integração: `ls /`, `/bin/echo absoluto`, `naoexiste` (127), `ls /nao-existe` (2), `cd /tmp` + `pwd`
- [ ] TT-02, `unset PATH` seguido de `ls` → 127; `/bin/ls` continua funcionando
- [ ] TT-03, Arquivo no `PATH` sem `+x` → `Permission denied` e 127 (RE-05, RE-06)
- [ ] TT-04, Cenários `@divergente` de `requirements.md` (ML-04, BUG-04, BUG-20, RE-01, BUG-03, BUG-16)
- [ ] TT-05, Teste unitário de `get_arg_size` para cada lexema
- [ ] TT-06, Valgrind com `--trace-children=yes` em 50 linhas mistas (builtin, externo, inexistente)

## Ordem Sugerida
1. T-01 → T-02 (estrutura e montagem; testáveis sem fork)
2. T-03 → T-04 → T-05 (resolução do binário)
3. T-06 → T-10 → T-07 → T-08 → T-09 (execução)

## Lacunas Pendentes (🔴)
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). Só para uma eventual reimplementação corretiva: semântica de comparação (`ft_strncmp` por prefixo), causa comum de BUG-04 e BUG-20
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). Só para uma eventual reimplementação corretiva: ML-04 (palavras depois de redirecionamento), RE-01 (builtin com redirecionamento), BUG-03 (`envp`), BUG-10 (status do builtin no filho), BUG-16 (126)
- ✅ L-03 (respondida em 2026-11-22): é um **bug** (BUG-03), confirmado pelo autor; documentado como comportamento atual (L-07)
