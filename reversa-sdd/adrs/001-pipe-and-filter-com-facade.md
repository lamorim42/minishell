# ADR-001: Arquitetura pipe-and-filter com Facade

- **Status:** Aceito (retroativo)
- **Data aproximada:** 2022-03 a 2022-05
- **Fontes:** README ("Estratégias utilizadas"), `src/facade.c`, histórico de `facade.c` (criado em 2022-05-11, commit `feat: new func in facade`) 🟢

## Contexto
O shell precisa transformar uma linha de texto em processos em execução, passando por várias etapas: tokenizar, validar, expandir, montar comandos, redirecionar e executar. Dois desenvolvedores trabalhavam em paralelo nessas etapas.

## Decisão
Organizar o processamento como uma sequência linear de **filtros**. Cada filtro lê e escreve campos de uma estrutura compartilhada (`t_line`), e a função `facade()` chama os filtros em ordem fixa. Cada etapa fica em uma pasta própria (`tokens/`, `exec/`) 🟢.

## Consequências
- ✅ Etapas pequenas, que podem ser testadas isoladamente. Os testes de `tokens/` aproveitam isso.
- ✅ É fácil inserir uma etapa nova; o heredoc foi encaixado entre a montagem e a execução.
- ⚠️ Os "filtros" não são puros: alteram `t_line` no lugar (`expand_var` reescreve `tks`) e dependem da ordem. 🟡
- ⚠️ A `t_line` virou um objeto central com 14 campos e acoplamento implícito entre as etapas. 🟢
