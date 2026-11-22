# ADR-010: Toolchain da 42 (clang, Make, Norminette, readline)

- **Status:** Aceito (restrição externa)
- **Datas:** 2022-03-17 (`add readline flag and libft`), 2022-03-30 (`change gcc to clang`), 2022-04-12 (`add gdb rule`), commits recorrentes `norminette` 🟢

## Contexto
Projeto avaliado pela 42, com restrições de ambiente e de estilo.

## Decisão
- `clang` com `-Wall -Wextra -Werror` 🟢
- Makefile próprio com `VPATH` e objetos em `obj/` 🟢
- GNU Readline (`-lreadline`) para edição de linha e histórico 🟢
- Libft como única biblioteca de apoio 🟢
- Conformidade com a Norminette: funções de até 25 linhas, até 5 variáveis, `static` para funções auxiliares, structs auxiliares (`s_hd`, `s_export`) para contornar o limite de variáveis 🟢
- Alvos de diagnóstico `make val` (Valgrind com `readline.supp`) e `make gdb` 🟢

## Consequências
- ✅ O build é reproduzível em qualquer máquina com clang e libreadline-dev.
- ⚠️ As versões não estão fixadas, e `readline.supp` não está versionado. 🔴
- ⚠️ As restrições da norma explicam parte da fragmentação em funções muito pequenas e dos nomes abreviados. 🟡
