# ADR-002: TDD com GoogleTest na tokenização

- **Status:** Aceito parcialmente (só no módulo `tokens`)
- **Data:** 2022-04-11 (`Init_tests_with_tdd`) → 2022-04-19 (`tests: New framework test (gtest)`) 🟢
- **Fontes:** README, `tests/`, commits de 11 a 21/04/2022 🟢

## Contexto
A tokenização de um shell tem muitos casos de borda (aspas encadeadas, operadores colados, variáveis). Os primeiros testes eram arquivos `.c` próprios (`tests/tests_minishell.c`, removido em 2022-04-16).

## Decisão
Escrever os testes antes da implementação para `count_tks`, `tokenizer`, `lexical_analyzer`, `sintax_analysis` e `clean_tokens`, usando **GoogleTest** (C++) com build separado em **CMake**, fora do Makefile da 42 🟢.

## Consequências
- ✅ Cerca de 84 casos protegem o tokenizador. É o módulo com melhor definição de comportamento.
- ⚠️ O TDD **não foi estendido** para `exec`, `builtins`, `hash_table` nem redirecionamentos. A maior parte dos bugs encontrados pelo Arqueólogo está nesses módulos. 🟢
- ⚠️ Os testes fazem `#include` direto de arquivos `.c` e o `CMakeLists` não lista `ft_strcmp.c`. Não há garantia de que a suíte ainda compila. 🟡
- ⚠️ O binário `tests/runTests` foi versionado durante um tempo (commit de 2022-05-28) e depois removido. 🟢
