# Executar Pipeline: Tarefas de Implementação

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Pré-requisitos
- [ ] Unit `executar-comando-simples` implementada (lista de nós, `init_fork`, `exec_path`, `exec_list`)
- [x] Decisão de L-07 sobre BUG-02 (builtin final no shell) → **L-07 decidida em 2026-11-22: documentar o as-is**

## Tarefas

- [ ] T-01, Criar um `pipe()` em `node->fd` para cada nó `PIPE`, no pai, antes dos forks
  - Origem no legado: `src/fd_handler_utils.c:15-28`
  - Critério de pronto: `ls | wc -l` imprime a contagem
  - Confiança: 🟢

- [ ] T-02, Em `find_input`, ligar `prev->fd[0]` ao stdin quando o nó anterior é `PIPE`
  - Origem no legado: `src/fd_handler.c:15-24`
  - Critério de pronto: `echo abc | cat` → `abc`
  - Confiança: 🟢

- [ ] T-03, Em `find_output`, ligar `pipe->fd[1]` ao stdout quando não há redirecionamento de saída antes do próximo `PIPE`
  - Origem no legado: `src/fd_handler.c:44-70`
  - Critério de pronto: `echo abc | cat | cat | wc -c` → `4`
  - Confiança: 🟢

- [ ] T-04, Em `close_fds` (filho), fechar as duas pontas de todos os `PIPE` e `HERE` da lista
  - Origem no legado: `src/fd_handler.c:72-95`
  - Critério de pronto: `yes | head -1` termina; `ls -l /proc/self/fd` dentro do pipeline mostra só 0, 1 e 2
  - Confiança: 🟢

- [ ] T-05, Em `init_fork` (pai), fechar o `PIPE` anterior depois de criar o comando à direita
  - Origem no legado: `src/exec/init_fork.c:25-29`
  - Critério de pronto: `cat | ls` não trava depois de `ls` terminar e do Ctrl-D no `cat`
  - Confiança: 🟢

- [ ] T-06, Esperar todos os filhos na ordem e usar o status do último em `$?`
  - Origem no legado: `src/exec/exec_list.c:42-69`
  - Critério de pronto: `ls /nope | true` → 0; `true | ls /nope` → 2
  - Confiança: 🟢

- [ ] T-07, Manter (as-is) ou corrigir a execução do builtin final no shell (BUG-02, PL-03)
  - Origem no legado: `src/exec/exec_list.c:30-32`
  - Critério de pronto (as-is): `ls | exit 3` encerra com 3. Critério de pronto (corrigido): o builtin roda no shell só quando a linha não tem `PIPE`
  - Confiança: 🟢
  - Nota: depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

## Tarefas de Teste

- [ ] TT-01, Cenários de `requirements.md`: dois comandos, pipeline longo, status do último, paralelismo, builtin no meio
- [ ] TT-02, Cenários `@divergente` (BUG-02, PL-03, `|` no fim)
- [ ] TT-03, Teste de vazamento de fd: 200 linhas `echo a | cat` seguidas; `ls /proc/<pid>/fd` do shell não cresce
- [ ] TT-04, Pipeline com 50 comandos `cat` (limite de fds)

## Ordem Sugerida
1. T-01 → T-02 → T-03 → T-04 → T-05 (ligação dos fds; validar com `echo | cat`)
2. T-06 (status)
3. T-07 só em uma reimplementação corretiva (L-07 = as-is)

## Lacunas Pendentes (🔴)
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). Só para uma eventual reimplementação corretiva: BUG-02 (builtin final no shell, inclusive `exit` e `cd`) e PL-03 (`$?` com builtin final)
