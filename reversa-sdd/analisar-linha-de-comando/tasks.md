# Analisar Linha de Comando: Tarefas de Implementação

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Pré-requisitos
- [ ] Unit `iniciar-sessao-e-ler-comando` implementada (entrega `line->str` com aspas balanceadas e sem ser comentário)
- [ ] Libft com `ft_strchr`, `ft_strncmp`, `ft_strlcpy`, `ft_strdup`, `ft_strtrim`, `ft_free_arr`
- [ ] GoogleTest e CMake disponíveis para as suítes de `tests/` (ADR-002)
- [x] Decisão de L-07 tomada: reproduzir as ⚠️ DIVERGÊNCIAS (as-is) ou corrigi-las. As tarefas abaixo descrevem o as-is e indicam onde a correção entraria → **L-07 decidida em 2026-11-22: documentar o as-is**

## Tarefas

- [ ] T-01, Declarar em `t_line` os campos `tks_nbr`, `tks`, `lex` e `ctks`, como arrays paralelos terminados em `NULL`
  - Origem no legado: `includes/minishell.h:63-67`, ADR-003
  - Critério de pronto: `init_line` zera os quatro campos; `free_line` libera os três arrays
  - Confiança: 🟢

- [ ] T-02, Implementar o corte de tokens (`tokenizer`, `worddup`, `make_red_len`, `make_quots_len`, `make_word_len`) conforme AT-01 a AT-07
  - Origem no legado: `src/tokens/tokenizer.c:20-113`
  - Critério de pronto: as 22 suítes de `tests/tests_tokenizer.cpp` passam; `ls>>file` → `ls`, `>>`, `file`; `a"b c"d` → um token; `"a"$X` → dois tokens
  - Confiança: 🟢

- [ ] T-03, Implementar `count_tks` com as mesmas regras de T-02, incluindo o decremento quando uma palavra continua em aspas
  - Origem no legado: `src/tokens/count_tks.c:21-93`
  - Critério de pronto: as 31 suítes de `tests/tests_count_tks.cpp` passam; para todo caso de T-02, `count_tks(s)` é igual ao tamanho de `tokenizer(s)`
  - Confiança: 🟢
  - Nota: numa reimplementação, vale derivar a contagem do próprio corte (uma única máquina de estados) para eliminar o risco de escrita fora do array 🟡

- [ ] T-04, Implementar `building_tokens` encadeando contagem, corte e classificação
  - Origem no legado: `src/tokens/tokenizer_utils.c:15-20`
  - Critério de pronto: depois da chamada, `tks_nbr`, `tks` e `lex` estão preenchidos e têm o mesmo tamanho
  - Confiança: 🟢

- [ ] T-05, Implementar `lexical_analyzer` / `find_lexema` com a ordem de LX-01 a LX-05
  - Origem no legado: `src/tokens/lexical_analyzer.c:17-51`
  - Critério de pronto: as 10 suítes de `tests/tests_lexical_analyzer.cpp` passam; `">"` → `WORD`; `$` → `WORD`; `'$HOME'` → `VAR`
  - Confiança: 🟢

- [ ] T-06, Implementar `sintax_analysis` com G-01, G-02 e G-03, aceitando `PIPE` no fim (RN-12)
  - Origem no legado: `src/tokens/sintax_analysis.c:17-44`
  - Critério de pronto: as 7 suítes de `tests/tests_sintax_analysis.cpp` passam; `| ls`, `ls | | cat`, `ls >`, `cat <<< x`, `> $F` → `FALSE`; `ls |` → `TRUE`
  - Confiança: 🟢
  - Nota: corrigir BUG-13 (aceitar `VAR` como alvo) é decisão de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-07, Implementar `free_sintax`: imprimir `Syntax ERROR!\n` em stdout, liberar `tks`, `lex` e `str`, sem alterar `status_code`
  - Origem no legado: `src/free.c:47-56`
  - Critério de pronto: cenário "Pipe no início"; Valgrind sem vazamento depois de 100 linhas inválidas
  - Confiança: 🟢
  - Nota: mudar para stderr e `$?=2` é bug (L-04 respondida); mantido as-is pela L-07

- [ ] T-08, Integrar na `facade`: `building_tokens` → `sintax_analysis` → (`free_sintax` | `exec_pipe_line`)
  - Origem no legado: `src/facade.c:18-30`
  - Critério de pronto: linha inválida não chega a `expand_var` nem a `add_history`
  - Confiança: 🟢

- [ ] T-09, Implementar `clean_tokens` / `clean_quots` / `clean_in_middle` conforme AQ-01 a AQ-05, chamado depois de `expand_var`
  - Origem no legado: `src/tokens/clean_tokens.c:18-87`, `src/facade.c:36-37`
  - Critério de pronto: as 13 suítes de `tests/tests_clean_tokens.cpp` passam; `"it's"` → `it's`; `a"b'c"` → `abc`; `"a"b"c"` → `a"b"c`
  - Confiança: 🟢
  - Nota: corrigir BUG-11 (remoção de aspas como no bash) depende de L-07 — ✅ decidida: manter o as-is; a correção fica fora do escopo atual

- [ ] T-10, Verificar o retorno de todos os `malloc` do módulo e tratar `tokenizer == NULL` em `building_tokens`
  - Origem no legado: `src/tokens/lexical_analyzer.c:23`, `src/tokens/clean_tokens.c:25,77`, `src/tokens/tokenizer_utils.c:18`
  - Critério de pronto: com falha de alocação simulada, o shell descarta a linha sem *segfault*
  - Confiança: 🟢 (lacuna de robustez do legado, não um comportamento observável)

## Tarefas de Teste

- [ ] TT-01, Rodar as 83 suítes GTest existentes (`tests/`) contra a nova implementação, sem alterar os casos
- [ ] TT-02, Teste de integração: `echo 'a | b'` imprime `a | b` e não cria pipe
- [ ] TT-03, Erros de sintaxe (`| ls`, `ls | | cat`, `echo oi >`, `cat <<`, `cat <<< x`) → `Syntax ERROR!` em stdout, nada executa, `echo $?` mostra o valor anterior
- [ ] TT-04, `echo ">"` imprime `>` e nenhum arquivo é criado
- [ ] TT-05, Linha só com espaços → nenhuma saída e nenhuma mensagem
- [ ] TT-06, Teste de propriedade: para strings aleatórias com aspas balanceadas sobre o alfabeto `a $ " ' | < > espaço`, `count_tks(s) == len(tokenizer(s))`
- [ ] TT-07, Cenários `@divergente` de `requirements.md` (`ls |`, `> $HOME/f`, `echo $X'q'`, `echo "a"b"c"`), que fixam o as-is (L-07 = documentar o comportamento atual)
- [ ] TT-08, Tab dentro da linha: `echo<TAB>oi` é um único token (RN-06)

## Ordem Sugerida
1. T-01 (estruturas)
2. T-02 → T-03 → T-04 (corte e contagem juntos, porque precisam coincidir; validar com TT-06 logo cedo)
3. T-05 → T-06 → T-07 → T-08 (classificação, gramática e integração na `facade`)
4. T-09 depende da unit `expandir-variaveis` só para o teste de integração; a função pode ser feita e testada isoladamente antes
5. T-10 por último, como endurecimento

## Lacunas Pendentes (🔴)
- ✅ L-07 decidida em 2026-11-22 (documentar o as-is). Só para uma eventual reimplementação corretiva: manter ou corrigir BUG-11 (remoção de aspas), BUG-13 (alvo `VAR` rejeitado), BUG-17 (`$X'q'` em dois argumentos), AT-04 (`"a"$X` em dois tokens) e RN-12 (`PIPE` final aceito)
- ✅ L-04 (respondida em 2026-11-22): **não foi deliberado**; é um bug (BUG-12/RN-13), documentado como comportamento atual (L-07)
- ✅ L-01 (respondida em 2026-11-22): o alvo é o **mínimo exigido pelo subject** da 42; tab como separador (RN-06) fica fora do escopo
