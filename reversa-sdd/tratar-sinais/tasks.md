# Tratar Sinais: Tarefas de Implementação

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Pré-requisitos
- [ ] GNU Readline com `rl_on_new_line`, `rl_replace_line`, `rl_redisplay`
- [ ] Global única `g_minishell` com ponteiros para a linha e a tabela (unit `iniciar-sessao-e-ler-comando`)
- [x] Decisão de L-07 sobre SG-02 e BUG-15 → **L-07 decidida em 2026-11-22: documentar o as-is**

## Tarefas

- [ ] T-01, Implementar `signals` / `sig_handler` (prompt: Ctrl-C limpa e redesenha; Ctrl-\ ignorado) e chamá-lo a cada iteração do laço
  - Origem no legado: `src/signals_utils.c:35-55`, `src/main.c:44`
  - Critério de pronto: cenários "Ctrl-C no prompt" e "Ctrl-\ no prompt"
  - Confiança: 🟢
  - Nota: gravar `$?=130` no handler (via a global) alinha com o bash; depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-02, Implementar `signal_ignore` em volta do fork do heredoc e `signal_here` / `sig_here` no filho
  - Origem no legado: `src/signals.c:33-38`, `src/signals_utils.c:17-33`, `src/facade.c:40-45`
  - Critério de pronto: Ctrl-C no heredoc encerra o filho com 130 (ver `ler-heredoc` T-06)
  - Confiança: 🟢

- [ ] T-03, Implementar `signals_parent` / `sig_parent` / `sig_write` antes de cada fork de comando
  - Origem no legado: `src/signals.c:17-45`, `src/exec/init_fork.c:15`
  - Critério de pronto: cenários "Interromper um comando" e "Quit em um comando"
  - Confiança: 🟢

- [ ] T-04, Restaurar o comportamento padrão dos sinais nos filhos antes do `execve` (hoje implícito) e decidir o comportamento dos builtins em filho
  - Origem no legado: `src/exec/init_fork.c:17-21` (SG-08, SG-09)
  - Critério de pronto: `echo a | cat` + Ctrl-C mata os dois processos
  - Confiança: 🟡

- [ ] T-05, Trocar as chamadas não *async-signal-safe* dos handlers (`printf`, `free`) por `write` e por um sinalizador lido fora do handler
  - Origem no legado: `src/signals_utils.c:23-33`
  - Critério de pronto: nenhum `printf`/`malloc`/`free` dentro de handler
  - Confiança: 🟢

## Tarefas de Teste

- [ ] TT-01, Testes por pseudo-terminal: `sleep 5` + `\x03` → `$?=130`; `sleep 5` + `\x1c` → `Quit` e `$?=131`; `cat | cat` + `\x03` → `130`
- [ ] TT-02, Ctrl-C no prompt com texto digitado → linha descartada (verificar com o readline real, não com um substituto)
- [ ] TT-03, Cenário `@divergente` SG-02

## Ordem Sugerida
1. T-01 (prompt)
2. T-03 (execução)
3. T-02 (heredoc, junto com `ler-heredoc`)
4. T-04 → T-05 (endurecimento)

## Lacunas Pendentes (🔴)
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). Só para uma eventual reimplementação corretiva: `$?=130` depois de Ctrl-C no prompt (SG-02), cancelamento do heredoc (BUG-15), texto `Quit (core dumped)`
