# Imprimir Texto (echo): Tarefas de Implementação

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Pré-requisitos
- [ ] Unit `executar-comando-simples` com o despacho de builtins
- [x] Decisão de L-07 sobre EC-04, EC-07 e BUG-14 → **L-07 decidida em 2026-11-22: documentar o as-is**

## Tarefas

- [ ] T-01, Implementar `echo_builtin` com o laço de `-n` exatos no início
  - Origem no legado: `src/builtins/echo.c:15-24`
  - Critério de pronto: cenários "Opção -n", "Várias opções -n" e "-n no meio é texto"
  - Confiança: 🟢
  - Nota: aceitar `-nnn` alinha com o bash; depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-02, Escrever os argumentos com um espaço entre eles, omitindo os vazios
  - Origem no legado: `src/builtins/echo.c:25-34`
  - Critério de pronto: `echo a b c` → `a b c`; `echo "" a` → `a` (as-is)
  - Confiança: 🟢
  - Nota: manter argumentos vazios (como o bash) só faz sentido junto com a correção do BUG-05; decidir as duas juntas em L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-03, Escrever `\n` no fim, exceto quando o primeiro argumento é `-n`; `echo` sem argumentos → `\n`
  - Origem no legado: `src/builtins/echo.c:20-21,35-36`
  - Critério de pronto: cenário "Sem argumentos"; `echo -n` não imprime nada
  - Confiança: 🟢

- [ ] T-04, Evitar a duplicação de saída (BUG-14): usar `write(1, …)` ou garantir `fflush(stdout)` antes de cada `fork`
  - Origem no legado: `src/builtins/echo.c`, `src/facade.c:40`, `src/exec/init_fork.c:16`
  - Critério de pronto: `printf 'echo -n x\nls\n' | ./minishell | od -c` mostra um único `x`
  - Confiança: 🟢

## Tarefas de Teste

- [ ] TT-01, Cenários de `requirements.md`, com `cat -e` para tornar visível o `\n`
- [ ] TT-02, Cenários `@divergente` (EC-04, EC-07)
- [ ] TT-03, Modo não interativo (stdout em pipe) para o BUG-14

## Ordem Sugerida
1. T-01 → T-02 → T-03
2. T-04

## Lacunas Pendentes (🔴)
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). Só para uma eventual reimplementação corretiva: `-nn`, argumentos vazios (junto com BUG-05), `printf` vs `write`
