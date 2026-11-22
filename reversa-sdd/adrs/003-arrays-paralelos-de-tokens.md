# ADR-003: Arrays paralelos de tokens em vez de pilha/lista ligada

- **Status:** Aceito (retroativo)
- **Data:** 2022-03-21 (`refactor: REMOVE all linked-list and stack files`, `refactor: start new approach`) 🟢

## Contexto
A primeira versão (17 a 20/03/2022) guardava os tokens em uma pilha e uma lista ligada (`src/data_struct/`, `linked_list`).

## Decisão
Abandonar essas estruturas e representar a linha como **arrays paralelos de strings** indexados pela posição do token: `tks[i]` (texto), `lex[i]` (categoria) e `ctks[i]` (texto limpo) 🟢.

## Consequências
- ✅ Tokenização e testes simples: comparar arrays com `ft_arrcmp`.
- ✅ A categoria léxica fica disponível por índice para a sintaxe e a montagem.
- ⚠️ É preciso contar os tokens antes (`count_tks`) para alocar. Contagem e tokenização são **duas implementações paralelas das mesmas regras**, e precisam ficar sincronizadas. Uma divergência resulta em escrita fora do array. 🟢
- ⚠️ As categorias são strings (`"PIPE"`), não `enum`, e são comparadas com `ft_strncmp` por prefixo em vários lugares. 🟢
