# Iniciar Sessão e Ler Comando: Tarefas de Implementação

## Pré-requisitos
- [ ] GNU Readline (headers e lib) disponível
- [ ] Unit `gerenciar-variaveis-de-ambiente` (tabela hash) implementada
- [ ] Unit `tratar-sinais` (pelo menos `signals()`) implementada

## Tarefas

- [ ] T-01, Definir `t_line` e `init_line` com todos os campos zerados (corrigir o zeramento de `fd`)
  - Origem no legado: `includes/minishell.h:60-76`, `src/init.c:15-28`
  - Critério de pronto: nenhum campo lido antes de ser inicializado (Valgrind sem "uninitialised")
  - Confiança: 🟢

- [ ] T-02, Carregar o `envp` na tabela preservando `=` nos valores e acrescentar `?`=`0`
  - Origem no legado: `src/hash_table/create_table.c:36-101`
  - Critério de pronto: `env` reproduz o ambiente herdado; `LS_COLORS` (com `=`) intacta
  - Confiança: 🟢

- [ ] T-03, Preencher a global `g_minishell` com os ponteiros da linha e da tabela
  - Origem no legado: `src/main.c:15,32-33`
  - Critério de pronto: os handlers de sinal conseguem acessar a linha e a tabela
  - Confiança: 🟢

- [ ] T-04, Guardar stdin e stdout (`dup`) antes do laço e restaurá-los (`dup2`) depois de cada linha
  - Origem no legado: `src/main.c:40-41,59-60`
  - Critério de pronto: cenário "Restauração de stdout"
  - Confiança: 🟢

- [ ] T-05, Implementar o laço `readline("miau> ")` com o tratamento de EOF (`exit` em stderr, código 0)
  - Origem no legado: `src/main.c:42-74`
  - Critério de pronto: cenário "Encerrar com Ctrl-D"; sem byte `\0` extra
  - Confiança: 🟢

- [ ] T-06, Implementar `verify_quots` e rejeitar a linha com `Syntax ERROR!`
  - Origem no legado: `src/tokens/sintax_analysis.c:46-81`, `src/main.c:47-52`
  - Critério de pronto: cenário "Aspas desbalanceadas"
  - Confiança: 🟢

- [ ] T-07, Implementar `is_a_comment` (espaços seguidos de `#`)
  - Origem no legado: `src/main.c:83-93`
  - Critério de pronto: cenário "Comentário"
  - Confiança: 🟢

- [ ] T-08, Despachar para `facade` e registrar no histórico as linhas executadas
  - Origem no legado: `src/main.c:58`, `src/facade.c:57`
  - Critério de pronto: a seta para cima recupera o comando
  - Confiança: 🟢

## Tarefas de Teste

- [ ] TT-01, Teste de integração: `printf 'echo oi\n' | ./minishell` produz `oi` (com o readline em modo não TTY)
- [ ] TT-02, EOF imediato → código de saída 0, stderr contém `exit`
- [ ] TT-03, Aspas desbalanceadas → `Syntax ERROR!` e nenhuma execução
- [ ] TT-04, Comentário → nenhuma saída
- [ ] TT-05, Valgrind: nenhum vazamento de `line->str` em 100 iterações

## Ordem Sugerida
1. T-01 → T-02 → T-03 (estado)
2. T-04 → T-05 (laço mínimo)
3. T-06 → T-07 → T-08 (filtros e despacho)

## Lacunas Pendentes (🔴)
- ✅ L-01 (respondida em 2026-11-22): o alvo é o **mínimo exigido pelo subject** da 42; modo não interativo fora do escopo
- ✅ L-04 (respondida em 2026-11-22): **não foi deliberado**; é um bug (BUG-12/RN-13), documentado como comportamento atual (L-07)
