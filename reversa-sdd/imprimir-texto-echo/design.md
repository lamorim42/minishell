# Imprimir Texto (echo): Design Técnico

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Interface

| Símbolo | Assinatura | Retorno | Observação |
|---------|-----------|---------|------------|
| `echo_builtin` | `(t_pipe_list *node)` | `void` | Lê `node->args`; não altera `$?` 🟢 |

## Fluxo Principal
`src/builtins/echo.c:15-37` 🟢:
```
i = 1
se args[1] == NULL: printf("\n")                        # echo sem argumentos
enquanto args[i] e args[i] == "-n": i++                 # pula as opções
enquanto args[i]:
   se args[i] não é vazio e args[i+1] == NULL:          # último argumento
       printf("%s", args[i]); pare
   se args[i] não é vazio: printf("%s ", args[i])       # vazio: nada, nem o espaço
   i++
se args[1] != NULL e args[1] != "-n": printf("\n")
```

Observações sobre o algoritmo 🟢:
- O separador é escrito **depois** de cada argumento não vazio que não é o último. Por isso um último argumento vazio deixa o espaço do penúltimo (`echo a ""` → `a `), e um vazio no meio some com o seu espaço (`echo a "" b` → `a b`)
- A decisão de imprimir `\n` olha só `args[1]`: basta o **primeiro** argumento ser `-n`
- A comparação com `-n` é exata (`ft_strcmp_len`), então `-nn` é texto

## Fluxos Alternativos
- **No shell** (último nó, sem redirecionamento): `printf` no processo do shell; o readline esvazia o buffer ao mostrar o próximo prompt 🟢
- **Em filho** (pipeline ou redirecionamento): `printf` no filho; `free_exit` → `exit` esvazia o buffer 🟢
- **stdout que não é terminal:** buffer completo; um `fork` posterior herda bytes pendentes e os escreve de novo ao sair (BUG-14) 🟢

## Dependências
- **executar-comando-simples:** `exec_builtins`
- **expandir-variaveis / analisar-linha-de-comando:** `args` já expandidos e sem aspas
- **libc:** `printf`

## Decisões de Design Identificadas

| Decisão | Evidência no código | Confiança |
|---------|---------------------|-----------|
| Saída por `printf` em vez de `write` | `src/builtins/echo.c` | 🟢 |
| Argumentos vazios descartados de propósito (provável efeito colateral do BUG-05, para que `$NOPE` não gere espaços) | `src/builtins/echo.c:28-33` | 🟡 |

## Estado Interno
Nenhum 🟢.

## Observabilidade
Nenhuma 🟢.

## Riscos e Lacunas
- 🟢 BUG-14: duplicação de saída pelo buffer do stdio herdado em `fork`
- 🟢 EC-04 (`-nn`), EC-07 (argumentos vazios)
- ✅ L-07 (decidida em 2026-11-22): documentar o as-is. Nota para uma eventual reimplementação corretiva, fora do escopo atual: trocar `printf` por `write` (ou `fflush` antes de cada `fork`) elimina o BUG-14
