# Encerrar Shell: Tarefas de Implementação

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Pré-requisitos
- [ ] Unit `executar-comando-simples` com o despacho de builtins e builtin no pai quando é o último nó
- [ ] `free_line` e `free_table` disponíveis
- [x] Decisão de L-07 sobre XT-01, XT-04 a XT-06, XT-10 e BUG-21 → **L-07 decidida em 2026-11-22: documentar o as-is**

## Tarefas

- [ ] T-01, Implementar a validação numérica separando "é número" do valor convertido
  - Origem no legado: `src/builtins/exit.c:17-37`
  - Critério de pronto: `+5`, `-1`, `256` válidos; `abc`, `5a`, `-`, `""` inválidos
  - Confiança: 🟢
  - Nota: no as-is, `-` e `""` saem com 0 (XT-05, XT-06); a correção depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-02, Implementar `exit_builtin`: sem argumento → 0 (as-is); 1 argumento numérico → `n & 0xFF`; não numérico → mensagem e 2; 2+ argumentos com o primeiro numérico → `too many arguments`, `$?=1`, sem sair
  - Origem no legado: `src/builtins/exit.c:40-59`
  - Critério de pronto: cenários de `requirements.md`; `exit 2 3` → `too many arguments` (corrigido) ou saída com 2 (as-is, BUG-21)
  - Confiança: 🟢
  - Nota: depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-03, Liberar `line` e `table` antes do `exit`
  - Origem no legado: `src/builtins/exit.c:56-58`
  - Critério de pronto: Valgrind sem "definitely lost" ao sair por `exit 5`
  - Confiança: 🟢

## Tarefas de Teste

- [ ] TT-01, Matriz de códigos: `exit`, `exit 7`, `exit -1`, `exit 256`, `exit +5`, `exit abc`, `exit 5a`, `exit -`, `exit ""`, `exit 2 3`, `exit 1 2`, `exit abc 3`, `exit 9999999999`, comparando com a tabela de `requirements.md`
- [ ] TT-02, `exit 3 | echo x` não encerra; `ls | exit 3` encerra (as-is)

## Ordem Sugerida
1. T-01 → T-02 → T-03

## Lacunas Pendentes (🔴)
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). Só para uma eventual reimplementação corretiva: `exit` sem argumento com o último `$?`; mensagens em stderr no formato do bash; `exit` escrito ao sair; BUG-21
