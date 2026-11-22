# Fluxograma: módulo `tokens`

> Arqueólogo · 2026-11-11 · 🟢 extraído de `src/tokens/*.c`

## Tokenização (`tokenizer` / `worddup`)

```mermaid
flowchart TD
    A([tokenizer]) --> B[count = count_tks str<br/>aloca tokens count+1]
    B --> C{*ptr?}
    C -- fim --> Z([tokens NULL-terminado])
    C -- espaço --> C1[ptr++] --> C
    C -- outro --> D{primeiro caractere}
    D -- "pipe, <, >" --> E["make_red_len: 1, ou 2 para >> e <<"]
    D -- aspa --> F[make_quots_len]
    D -- outro --> G[make_word_len]
    F --> F1[até a aspa de fechamento]
    F1 --> F2{próximo caractere}
    F2 -- "pipe, >, <, $, espaço ou fim" --> H
    F2 -- aspa --> F
    F2 -- outro --> F3[len++] --> F2
    G --> G1{caractere}
    G1 -- "pipe, >, <, espaço ou fim" --> H
    G1 -- aspa e palavra NÃO começa com $ --> F
    G1 -- aspa e palavra começa com $ --> H
    G1 -- outro --> G2[len++] --> G1
    E --> H[strlcpy token, ptr += len] --> C
```

## Classificação léxica e sintaxe

```mermaid
flowchart TD
    A([token]) --> B{"começa com pipe?"}
    B -- sim --> P[PIPE]
    B -- não --> C{== '>>'?}
    C -- sim --> RA[REDA]
    C -- não --> D{== '<<'?}
    D -- sim --> HE[HERE]
    D -- não --> E{começa com '>'?}
    E -- sim --> RO[REDO]
    E -- não --> F{começa com '<'?}
    F -- sim --> RI[REDI]
    F -- não --> G{contém '$' e len > 1?}
    G -- sim --> V[VAR]
    G -- não --> W[WORD]
```

```mermaid
flowchart TD
    S([sintax_analysis lex]) --> A{lex 0 == PIPE?}
    A -- sim --> X[FALSE]
    A -- não --> L{para cada lex i}
    L --> B{PIPE seguido de PIPE?}
    B -- sim --> X
    B -- não --> C{REDO/REDA/REDI/HERE<br/>e próximo != WORD?}
    C -- sim --> X
    C -- não --> L
    L -- fim --> OK[TRUE<br/>obs.: PIPE no final é aceito]
```

## Remoção de aspas (`clean_tokens`)

```mermaid
flowchart TD
    A([token]) --> B{tem aspa?}
    B -- não --> C[strdup]
    B -- sim --> D{começa e termina com aspas duplas?}
    D -- sim --> E[strtrim das aspas duplas nas pontas]
    D -- não --> F{começa e termina com aspas simples?}
    F -- sim --> G[strtrim das aspas simples nas pontas]
    F -- não --> H[clean_in_middle:<br/>remove TODAS as aspas]
```
