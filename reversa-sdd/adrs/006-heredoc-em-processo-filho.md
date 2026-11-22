# ADR-006: Heredoc lido em processo filho e entregue por pipe

- **Status:** Aceito (retroativo)
- **Datas:** 2022-07-03 (`init here_doc functions`) → 2022-07-17/18 (`fix: here_doc signals`, PR #29 `fix/here_doc`, `refactor here_doc fd`) 🟢

## Contexto
Durante a digitação do heredoc, Ctrl-C precisa interromper a leitura **sem matar o shell**. Com `readline`, interromper a leitura no próprio processo exige manipular o estado da biblioteca.

## Decisão
Antes de executar, o pai cria um `pipe()` para cada `<<` e faz **fork** de um filho. O filho instala um handler de SIGINT que chama `exit(130)`, lê todos os heredocs com `readline("> ")` e escreve cada conteúdo na ponta de escrita do pipe correspondente. O pai espera o filho, fecha as pontas de escrita e mais tarde usa `fd[0]` como stdin do comando 🟢.

## Consequências
- ✅ Ctrl-C mata só o filho; o shell continua vivo.
- ⚠️ **O fork acontece em toda linha**, mesmo sem heredoc. É um custo pequeno, mas tem dois efeitos colaterais: o buffer de stdio é duplicado (BUG-14) e o `$?` intermediário é sobrescrito. 🟢
- ⚠️ O status 130 do filho **não cancela** a linha. 🟡
- ⚠️ O conteúdo fica parado no pipe até a execução. Um heredoc maior que o buffer do pipe (~64 KiB) trava o filho. 🟡
- Não há expansão de variáveis no heredoc. 🟢
