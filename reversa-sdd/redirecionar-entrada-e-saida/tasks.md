# Redirecionar Entrada e Saída: Tarefas de Implementação

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Pré-requisitos
- [ ] Unit `executar-comando-simples` implementada (lista de nós com `union` `bin`/`fd`)
- [ ] Unit `executar-pipeline` implementada (ramos `PIPE` de `find_input`/`find_output`)
- [x] Decisão de L-07 sobre o modelo de redirecionamento (posicional, as-is, ou por comando, como no bash) → **L-07 decidida em 2026-11-22: documentar o as-is**

## Tarefas

- [ ] T-01, Implementar `open_fds` com os modos de RD-01 a RD-03
  - Origem no legado: `src/fd_handler_utils.c:15-44`
  - Critério de pronto: cenários "Truncar e acrescentar" e "Último redirecionamento vence"
  - Confiança: 🟢

- [ ] T-02, Tratar falha de abertura de arquivo de saída **sem** derrubar o shell: `perror(alvo)`, descartar a linha, `$?=1`
  - Origem no legado: `src/fd_handler_utils.c:39-43` (BUG-01)
  - Critério de pronto: `echo x > /nonexistent/x` → mensagem e prompt de volta
  - Confiança: 🟢
  - Nota: ✅ P-7 (respondida em 2026-11-22): numa reimplementação, este caso deve ser **corrigido**; o critério de pronto desta tarefa é a correção, e o as-is fica documentado em `requirements.md`

- [ ] T-03, Implementar `find_input` (anterior imediato + varredura para frente até `PIPE`, último vence, erro com `-1`)
  - Origem no legado: `src/fd_handler.c:15-42`
  - Critério de pronto: `cat < in1`, `< in1 cat`, `echo z | cat < in1` → `conteudo`; `cat < nope` → mensagem e `$?=1`
  - Confiança: 🟢
  - Nota: no as-is, `< nope cat` roda sem erro (BUG-09); a correção depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-04, Implementar `find_output` (varredura para frente, último vence, `PIPE` como padrão)
  - Origem no legado: `src/fd_handler.c:44-70`
  - Critério de pronto: `echo a > f5 | cat` → `cat` não recebe nada; `cat < in1 > o2` → `o2` = `conteudo`
  - Confiança: 🟢
  - Nota: no as-is, `> f4 echo d` vai para o terminal (BUG-08); a correção depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-05, Implementar `close_fds` e o fechamento de `REDO`/`REDA` no pai (`init_fork`)
  - Origem no legado: `src/fd_handler.c:72-95`, `src/exec/init_fork.c:22-24`
  - Critério de pronto: nenhum fd extra aberto no filho (`ls -l /proc/self/fd`)
  - Confiança: 🟢

- [ ] T-06, Fechar também os fds de `REDI` no pai ao fim da linha
  - Origem no legado: ausência em `src/exec/exec_list.c:27` (RR-08)
  - Critério de pronto: 200 linhas `cat < in1` não aumentam os fds abertos do shell
  - Confiança: 🟢

- [ ] T-07, Substituir a checagem `file_exists(fd)` em nós que não são de entrada por uma checagem só de nós `REDI`/`HERE`
  - Origem no legado: `src/fd_handler.c:18-24` (RR-07)
  - Critério de pronto: `cat < in1 -e` tem comportamento determinístico em 100 execuções
  - Confiança: 🟡

## Tarefas de Teste

- [ ] TT-01, Cenários de `requirements.md` (truncar, acrescentar, último vence, entrada, prioridade sobre o pipe, entrada inexistente)
- [ ] TT-02, Cenários `@divergente` (BUG-08, BUG-09, RR-04) (L-07 = as-is: estes cenários são o comportamento esperado)
- [ ] TT-03, Permissões: arquivo criado tem modo `0644 & ~umask`
- [ ] TT-04, Crash (BUG-01): o teste deve **falhar** no legado e passar na reimplementação

## Ordem Sugerida
1. T-01 → T-02 (abertura segura)
2. T-03 → T-04 → T-05 (ligação no filho)
3. T-06 → T-07 (endurecimento)

## Lacunas Pendentes (🔴)
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). Só para uma eventual reimplementação corretiva: modelo posicional (as-is) ou por comando (bash), que decide BUG-08, BUG-09, RR-04 e ML-04
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). P-7 respondida: o crash (BUG-01) deve ser corrigido numa reimplementação (T-02)
