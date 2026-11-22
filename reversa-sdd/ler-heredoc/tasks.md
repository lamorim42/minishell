# Ler Heredoc: Tarefas de Implementação

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Pré-requisitos
- [ ] Units `executar-comando-simples` e `redirecionar-entrada-e-saida` implementadas
- [ ] Unit `tratar-sinais` com `signal_ignore` e `sig_here`
- [ ] GNU Readline disponível
- [x] Decisão de L-07 sobre expansão no conteúdo, Ctrl-C e o limite do pipe → **L-07 decidida em 2026-11-22: documentar o as-is**

## Tarefas

- [ ] T-01, Implementar `creat_here_doc` (um `pipe()` por nó `HERE`)
  - Origem no legado: `src/exec/here_doc_utils.c:42-54`
  - Critério de pronto: cada nó `HERE` tem `fd[0]` e `fd[1]` válidos
  - Confiança: 🟢

- [ ] T-02, Implementar a leitura (`here_doc_buffer`, `generation_content`, `putting_nl`) com o prompt `> ` e a comparação exata do delimitador
  - Origem no legado: `src/exec/here_doc.c:27-80`
  - Critério de pronto: cenários "Conteúdo literal" e "Delimitador com aspas"; `FIMX` não encerra o heredoc `FIM`
  - Confiança: 🟢
  - Nota: expandir `$` quando o delimitador não tem aspas alinha com o bash; depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-03, Tratar EOF com `<DELIM>: Wanted` e manter o conteúdo parcial
  - Origem no legado: `src/exec/here_doc.c:40-44`
  - Critério de pronto: cenário "EOF antes do delimitador"
  - Confiança: 🟢

- [ ] T-04, Implementar `here_doc_verification` e `here_doc_write` no filho, na ordem da linha
  - Origem no legado: `src/exec/here_doc_utils.c:15-40`
  - Critério de pronto: cenário "Dois heredocs"
  - Confiança: 🟢

- [ ] T-05, Integrar o fork do heredoc na `facade` (`signal_ignore` no pai, `signal_here` no filho, `waitpid`, `close_here_doc`)
  - Origem no legado: `src/facade.c:38-52`
  - Critério de pronto: cenário "Heredoc com pipe"; nenhum fd de escrita do heredoc aberto no pai depois do `waitpid`
  - Confiança: 🟢

- [ ] T-06, Tratar Ctrl-C: cancelar a linha inteira quando o filho sair com 130 e gravar `$?=130`
  - Origem no legado: `src/facade.c:51` (status ignorado, BUG-15), `src/signals_utils.c:23-33`
  - Critério de pronto (corrigido): Ctrl-C no heredoc volta ao prompt sem executar o comando. Critério de pronto (as-is): o comando executa com stdin vazio
  - Confiança: 🟢
  - Nota: depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-07, Evitar o travamento com conteúdo grande (arquivo temporário, ou leitura do pipe em paralelo)
  - Origem no legado: `src/exec/here_doc_utils.c:34-40` (RE-09)
  - Critério de pronto: heredoc de 200 KiB chega inteiro ao `wc -c`
  - Confiança: 🟢
  - Nota: ✅ P-7 (respondida em 2026-11-22): numa reimplementação, este caso deve ser **corrigido**; o critério de pronto desta tarefa é a correção, e o as-is fica documentado em `requirements.md`

## Tarefas de Teste

- [ ] TT-01, Testes por pseudo-terminal (`pty`): os cenários de `requirements.md`, enviando linhas, `\x04` (Ctrl-D) e `\x03` (Ctrl-C)
- [ ] TT-02, Cenários `@divergente` (BUG-15, RN-27) (L-07 = as-is: estes cenários são o comportamento esperado)
- [ ] TT-03, Conteúdo com aspas e `$` preservado literalmente
- [ ] TT-04, Heredoc de 100 KiB (RE-09): no as-is trava (observado na revisão)

## Ordem Sugerida
1. T-01 → T-02 → T-03 → T-04 (leitura isolada, testável sem fork)
2. T-05 (integração)
3. T-06 → T-07 só em uma reimplementação corretiva (L-07 = as-is)

## Lacunas Pendentes (🔴)
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). Só para uma eventual reimplementação corretiva: expansão no conteúdo (RN-27), cancelamento por Ctrl-C (BUG-15), limite do pipe (RE-09)
