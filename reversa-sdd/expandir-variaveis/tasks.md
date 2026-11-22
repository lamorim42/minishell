# Expandir Variáveis: Tarefas de Implementação

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Pré-requisitos
- [ ] Unit `analisar-linha-de-comando` implementada (fornece `tks` e `lex`; consome o resultado em `clean_tokens`)
- [ ] Tabela de ambiente com `search_item` que devolve uma **cópia** do valor (unit `gerenciar-variaveis-de-ambiente`)
- [ ] Chave `?` inserida com `0` no início da sessão (unit `iniciar-sessao-e-ler-comando`)
- [x] Decisão de L-07 tomada: reproduzir as ⚠️ DIVERGÊNCIAS (as-is) ou corrigi-las. As tarefas abaixo descrevem o as-is e indicam onde a correção entraria → **L-07 decidida em 2026-11-22: documentar o as-is**

## Tarefas

- [ ] T-01, Implementar `return_var_key` / `key_len` com o conjunto de delimitadores `" ' , : . / [ ] { } = - + * ! @ #` e espaço, sem tratar `$` como delimitador
  - Origem no legado: `src/exec/expand_var_utils.c:15-49`
  - Critério de pronto: `return_var_key("pre$HOME/pos")` → `HOME`; `"$HOME$USER"` → `HOME$USER`; `"$?abc"` → `?abc`; `"\"$\""` → `""`
  - Confiança: 🟢
  - Nota: incluir `$` entre os delimitadores (e limitar `$?` a um caractere) corrige BUG-05 parcial e `$?abc`; depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-02, Implementar `take_prefix_var` e `sufix_var`
  - Origem no legado: `src/exec/exec_utils.c:24-48`
  - Critério de pronto: `take_prefix_var("ab$X", &i)` → `ab` com `i == 2`; sem prefixo → `NULL` com `i == 0`; `sufix_var` sem resto → `NULL`
  - Confiança: 🟢

- [ ] T-03, Implementar `join_str_value` / `join_helper` montando `prefixo + valor + sufixo`, com o atalho para o token igual a `$KEY`
  - Origem no legado: `src/exec/expand_var.c:57-93`
  - Critério de pronto: `pre$HOME/pos` → `pre/home/u/pos`; `$HOME` → `/home/u`; `"$HOME $USER"` → `"/home/u $USER"` depois da primeira chamada
  - Confiança: 🟢

- [ ] T-04, Implementar `change_value`: buscar a chave; se existir, juntar; se não existir, trocar o **token inteiro** por `""` (BUG-05, as-is)
  - Origem no legado: `src/exec/expand_var.c:33-55`
  - Critério de pronto: cenário "Variável inexistente apaga o token inteiro"; `Z=` → `echo a$Z` imprime `a`; Valgrind sem vazamento de `key`, `value` ou do token antigo
  - Confiança: 🟢
  - Nota: substituir só o trecho `$NOME` por `""` alinha com o bash; depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-05, Implementar `expand_var`: percorrer `lex`, expandir só `VAR` cujo `tks[i][0] != '\''`, repetindo enquanto houver `$`
  - Origem no legado: `src/exec/expand_var.c:19-31`
  - Critério de pronto: cenários "Variável simples", "Aspas simples impedem a expansão", "Cifrão isolado é literal"; `a'$HOME'` → `a/home/u` (as-is); `'a'"$HOME"` → `a$HOME` (as-is)
  - Confiança: 🟢
  - Nota: um modelo de aspas por caractere (estado dentro/fora de `'` e `"`) corrigiria as duas divergências; depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-06, Proteger o laço de reexpansão contra valores auto-referentes
  - Origem no legado: `src/exec/expand_var.c:27-28` (RX-06)
  - Critério de pronto: com `A='$A'` herdado, `echo $A` termina (imprime `$A` como o bash, ou outro resultado definido), sem travar
  - Confiança: 🟢 (falha observada)
  - Nota: ✅ P-7 (respondida em 2026-11-22): numa reimplementação, este caso deve ser **corrigido**; o critério de pronto desta tarefa é a correção, e o as-is fica documentado em `requirements.md`

- [ ] T-07, Integrar na `facade`: `expand_var` depois de `sintax_analysis` e antes de `clean_tokens`
  - Origem no legado: `src/facade.c:36-37`
  - Critério de pronto: `echo "$HOME"` → `/home/u` sem aspas; `Q='"q"'` → `echo $Q` → `q` (RN-26, as-is)
  - Confiança: 🟢

- [ ] T-08, Garantir que `$?` reflete `update_status_code` da execução anterior
  - Origem no legado: `src/exec/exec_list.c:54-69`
  - Critério de pronto: `ls /nao-existe` seguido de `echo $?` → `2`; comando inexistente seguido de `echo $?` → `127`; processo morto por SIGINT → `130`
  - Confiança: 🟢

## Tarefas de Teste

- [ ] TT-01, Teste unitário de `return_var_key` para cada delimitador do conjunto e para `$`, `?`, `_`, dígitos e `{`
- [ ] TT-02, Teste unitário de `expand_var` com a tabela preenchida: os 28 casos executados pelo Redator (ver `requirements.md`), com o resultado de `ctks` como oráculo
- [ ] TT-03, Teste de integração: `echo "$HOME $USER"`, `echo pre$HOME/pos`, `echo '$HOME'`, `echo $`, `export A=$HOME` seguido de `env | grep ^A=`
- [ ] TT-04, Cenários `@divergente` de `requirements.md` (`"a $NOPE b"`, `$HOME$USER`, `a'$HOME'`, `$Q`), que fixam o as-is (L-07 = documentar o comportamento atual)
- [ ] TT-05, Teste com `timeout`: `A='$A' ./minishell` e `echo $A` deve terminar em menos de 1 s (valida T-06)
- [ ] TT-06, Valgrind: nenhuma perda de memória em 100 linhas com 3 variáveis cada, existentes e inexistentes
- [ ] TT-07, Sem *word splitting*: `V='a b'` → um comando externo de teste (`printf '<%s>\n' $V`) recebe um argumento só (RN-25, as-is)

## Ordem Sugerida
1. T-01 → T-02 → T-03 (funções puras, testáveis isoladamente com TT-01)
2. T-04 → T-05 (dependem da tabela hash)
3. T-06 logo em seguida, antes de qualquer teste com ambiente herdado arbitrário
4. T-07 → T-08 (integração com a `facade` e com `exec_list`)

## Lacunas Pendentes (🔴)
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). Só para uma eventual reimplementação corretiva: preservar ou corrigir BUG-05 (token inteiro apagado; `$A$B`), a reexpansão de valores, a remoção das aspas vindas do valor (RN-26), a proteção por aspas simples só no início do token e `$?abc`
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). P-7 respondida: o travamento deve ser corrigido numa reimplementação (T-06)
- ✅ L-01 (respondida em 2026-11-22): o alvo é o **mínimo exigido pelo subject** da 42; *word splitting*, `${NOME}`, `$$` e expansão no heredoc ficam fora do escopo
