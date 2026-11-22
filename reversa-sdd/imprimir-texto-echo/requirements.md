# Imprimir Texto (echo)

> Unit de caso de uso · Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash
> Casos "(observado)" foram executados contra o `main.c` legado compilado na scratchpad, com um `readline` substituto que esvazia o stdout antes de ler (como o readline real faz ao mostrar o prompt). Saída inspecionada com `cat -A`. O repositório não foi alterado.

## Visão Geral
Implementa o builtin `echo`: escreve os argumentos separados por espaço, seguidos de `\n`, com a opção `-n` para omitir a quebra de linha. A saída usa `printf` (stdio com buffer) 🟢.

## Responsabilidades
- Escrever os argumentos separados por um espaço 🟢
- Reconhecer uma ou mais opções `-n` no início 🟢
- Omitir os argumentos vazios 🟢
- Terminar com `\n`, exceto com `-n` 🟢

## Regras de Negócio
- EC-01: `echo` sem argumentos → `\n` 🟢 (observado)
- EC-02: `echo a b c` → `a b c\n` 🟢 (observado)
- EC-03: `-n` é reconhecido só como **argumento exatamente igual** a `-n`, em qualquer quantidade no início: `echo -n x` → `x`; `echo -n -n y` → `y` 🟢 (observado)
- EC-04: `-nn` não é opção: `echo -nn z` → `-nn z\n` 🟢 (observado). ⚠️ O bash aceita `-nn` como `-n`
- EC-05: `-n` depois de um argumento comum é texto: `echo a -n b` → `a -n b` 🟢 (observado)
- EC-06: `echo -n` sozinho não imprime nada 🟢 (observado)
- EC-07: argumentos **vazios** são omitidos, inclusive o espaço: `echo "" a` → `a`; `echo a "" b` → `a b` 🟢 (observado). ⚠️ O bash dá ` a` e `a  b`
- EC-08: argumento vazio no fim deixa um espaço sobrando: `echo a ""` → `a \n` 🟢 (observado), igual ao bash
- EC-09: `echo $NOPE` (token expandido para vazio) → só `\n` 🟢 (observado)
- EC-10: não há `-e` nem `-E`; o texto sai literal: `echo -e "a\nb"` → `-e a\nb` 🟢 (observado). ⚠️ O `echo` do bash interpreta `-e`
- EC-11: espaços dentro de aspas são preservados: `echo "a  b"` → `a  b` 🟢 (observado)
- EC-12: funciona em pipeline e com redirecionamento (roda em filho): `echo x | cat -e` → `x$`; `echo a > ob` grava `a\n` 🟢 (observado)
- BUG-14: como a saída usa `printf`, o buffer do stdio pode ser herdado pelos `fork` seguintes e **duplicar** a saída quando o stdout não é um terminal (pipe ou arquivo) 🟢 (ver `code-analysis.md` §9)
- ✅ L-07 (decidida em 2026-11-22, [Revisão]): **documentar o comportamento atual**. As ⚠️ DIVERGÊNCIAS desta unit são o comportamento especificado, não defeitos a corrigir. (Pendência original: EC-04, EC-07 e BUG-14)

## Requisitos Funcionais

| ID | Requisito | Prioridade | Critério de Aceite |
|----|-----------|-----------|-------------------|
| RF-01 | Escrever os argumentos separados por um espaço e `\n` no fim | Must | `echo a b c` → `a b c` |
| RF-02 | Aceitar `-n` (uma ou mais vezes no início) e omitir o `\n` | Must | `echo -n x \| cat -e` → `x` sem `$` |
| RF-03 | Tratar `-n` fora do início como texto | Must | `echo a -n b` → `a -n b` |
| RF-04 | `echo` sem argumentos imprime uma linha vazia | Must | `echo \| cat -e` → `$` |
| RF-05 | Omitir argumentos vazios (as-is) | Should | `echo "" a` → `a` |

## Requisitos Não Funcionais

| Tipo | Requisito inferido | Evidência no código | Confiança |
|------|--------------------|---------------------|-----------|
| E/S | Saída pelo stdio (`printf`), com buffer; não usa `write` direto | `src/builtins/echo.c:15-37` | 🟢 |

## Critérios de Aceitação

```gherkin
Funcionalidade: echo

  Cenário: Argumentos simples
    Quando o usuário digita "echo a b c"
    Então a saída é "a b c" seguida de quebra de linha

  Cenário: Opção -n
    Quando o usuário digita "echo -n x | cat -e"
    Então a saída é "x" sem "$"

  Cenário: Várias opções -n
    Quando o usuário digita "echo -n -n y | cat -e"
    Então a saída é "y" sem "$"

  Cenário: -n no meio é texto
    Quando o usuário digita "echo a -n b"
    Então a saída é "a -n b"

  Cenário: Sem argumentos
    Quando o usuário digita "echo | cat -e"
    Então a saída é "$"

  @divergente
  Cenário: -nn não é opção (EC-04)
    Quando o usuário digita "echo -nn z"
    Então a saída é "-nn z"

  @divergente
  Cenário: Argumento vazio omitido (EC-07)
    Quando o usuário digita 'echo a "" b'
    Então a saída é "a b"
```

## Prioridade (MoSCoW)

| Requisito | MoSCoW | Justificativa |
|-----------|--------|---------------|
| RF-01 a RF-04 | Must | `echo` com `-n` é obrigatório no subject |
| RF-05 | Should | Divergente; mantido por fidelidade ao as-is |

## Rastreabilidade de Código

| Arquivo | Função | Cobertura |
|---------|--------|-----------|
| `src/builtins/echo.c` | `echo_builtin` | 🟢 |
| `src/builtins/builtins_utils.c` | `exec_builtins` (despacho) | 🟢 |
| `src/tokens/tokenizer_utils.c` | `ft_strcmp_len` (comparação exata com `-n`) | 🟢 |
