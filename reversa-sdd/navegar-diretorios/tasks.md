# Navegar Diretórios: Tarefas de Implementação

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Pré-requisitos
- [ ] Unit `gerenciar-variaveis-de-ambiente` implementada (`search_item`, `update_var_env`)
- [ ] Unit `executar-comando-simples` com builtin no pai quando é o último nó
- [x] Decisão de L-07 sobre `pwd` (variável ou diretório real) e sobre as mensagens do `cd` → **L-07 decidida em 2026-11-22: documentar o as-is**

## Tarefas

- [ ] T-01, Implementar `update_var_env` (apagar se existir e inserir)
  - Origem no legado: `src/builtins/cd.c:42-51`
  - Critério de pronto: `PWD` atualizado depois de 100 `cd` alternados, sem duplicatas na tabela
  - Confiança: 🟢

- [ ] T-02, Implementar `cd <dir>` com `getcwd` antes e depois, `PWD` e `OLDPWD`
  - Origem no legado: `src/builtins/cd.c:19-40`
  - Critério de pronto: cenários "Mudar de diretório" e "Caminho com '..' é resolvido"
  - Confiança: 🟢
  - Nota: usar `getcwd(NULL, 0)` (alocação dinâmica) elimina o limite de 500 bytes 🟡

- [ ] T-03, Implementar `cd` e `cd ~` via `HOME`, com `cd: HOME not set`
  - Origem no legado: `src/builtins/cd.c:24-25,53-72`
  - Critério de pronto: cenário "cd sem argumento"; `unset HOME`; `cd` → mensagem
  - Confiança: 🟢
  - Nota: gravar `$?=1` em `HOME not set` (BUG-12) e `PWD` resolvido depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-04, Tratar falha de `chdir` com `perror(<dir>)` e `$?=1`
  - Origem no legado: `src/builtins/cd.c:26-30`
  - Critério de pronto: cenário "Diretório inexistente"
  - Confiança: 🟢

- [ ] T-05, Implementar `pwd` imprimindo `PWD` da tabela
  - Origem no legado: `src/builtins/pwd.c:15-22`
  - Critério de pronto (as-is): cenários `@divergente` de `pwd`. Critério de pronto (corrigido): `pwd` = `getcwd()`
  - Confiança: 🟢
  - Nota: depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

## Tarefas de Teste

- [ ] TT-01, Cenários de `requirements.md`
- [ ] TT-02, Cenários `@divergente` (BUG-12, `cd -`, `pwd` com `PWD` falso ou ausente)
- [ ] TT-03, `cd` com redirecionamento (`cd /tmp > /dev/null`) não muda o diretório (as-is RE-01)
- [ ] TT-04, Caminho com mais de 500 bytes: documentar o comportamento

## Ordem Sugerida
1. T-01 → T-02 → T-04 (núcleo do `cd`)
2. T-03 (`HOME`)
3. T-05 (`pwd`)

## Lacunas Pendentes (🔴)
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). Só para uma eventual reimplementação corretiva: `pwd` pela variável ou pelo diretório real; `$?` do `HOME not set`; suporte a `cd -` e `~/`
