# Relatório de Confiança — minishell

> Gerado pelo Revisor (Reversa) em 2026-11-22 · versão 2 (final), depois das respostas de `questions.md`
> Decisões aplicadas: **L-07 = documentar o comportamento atual** (as ⚠️ DIVERGÊNCIAS em relação ao bash são o comportamento especificado) e **L-01 = mínimo do subject** (recursos além dele ficam fora do escopo).

---

## Resumo Geral

Contagem de marcas de confiança em todas as afirmações das 12 units (`requirements.md`, `design.md`, `tasks.md`), das 2 user stories e da code-spec matrix. Legendas e títulos ficam fora da contagem.

| Nível | Quantidade | Percentual |
|-------|-----------|------------|
| 🟢 CONFIRMADO | 877 | 95,7% |
| 🟡 INFERIDO   | 39 | 4,3% |
| 🔴 LACUNA     | 0 | 0,0% |
| **Total**     | 916 | 100% |

**Confiança geral:** 97,9% (🟢 + metade dos 🟡)

Os 🟢 têm duas fontes: a leitura do código (arquivo e linha) e a execução do código original compilado na scratchpad, com mais de 150 casos marcados "(observado)". As 9 perguntas de `questions.md` foram respondidas.

---

## Por Spec

| Spec | 🟢 | 🟡 | 🔴 | Confiança |
|------|----|----|-----|-----------|
| `iniciar-sessao-e-ler-comando/` | 52 | 3 | 0 | 97% |
| `analisar-linha-de-comando/` | 99 | 4 | 0 | 98% |
| `expandir-variaveis/` | 84 | 3 | 0 | 98% |
| `executar-comando-simples/` | 86 | 3 | 0 | 98% |
| `executar-pipeline/` | 57 | 2 | 0 | 98% |
| `redirecionar-entrada-e-saida/` | 66 | 3 | 0 | 98% |
| `ler-heredoc/` | 62 | 1 | 0 | 99% |
| `gerenciar-variaveis-de-ambiente/` | 85 | 2 | 0 | 99% |
| `navegar-diretorios/` | 43 | 5 | 0 | 95% |
| `imprimir-texto-echo/` | 37 | 1 | 0 | 99% |
| `encerrar-shell/` | 41 | 0 | 0 | 100% |
| `tratar-sinais/` | 50 | 5 | 0 | 95% |
| `user-stories/uso-interativo.md` | 30 | 0 | 0 | 100% |
| `user-stories/automacao-com-pipes-e-redirecionamentos.md` | 29 | 0 | 0 | 100% |
| `traceability/code-spec-matrix.md` | 56 | 7 | 0 | 94% |

---

## Lacunas Pendentes 🔴

Nenhuma. Os 39 itens 🟡 que continuam inferidos estão em `gaps.md` §2; nenhum bloqueia a documentação.

---

## Revisão por unit e cruzada

- As 12 units têm os 3 arquivos canônicos; os 8 módulos de `surface.json` estão cobertos 🟢
- `code-spec-matrix.md`: 44/44 arquivos de `src/` + `includes/` têm unit; os 31 n/a são build ou Libft não usada 🟢
- `spec-impact-matrix.md` (Arquiteto): as dependências conferem com o código. Uma dependência transversal não aparece: o `ft_strncmp` da Libft com semântica de prefixo afeta Léxico, Sintaxe, Lista, Executor, Ligação de fds e Builtins (causa do BUG-04 e do BUG-20) 🟢
- Contradição corrigida: em `ls | > o1 wc -l`, o `wc` **não** lê `o1`; fica com o stdin original (`src/fd_handler.c:9-12`) 🟢
- Consistência do `$?` entre units: o `waitpid` do filho do heredoc, que roda em toda linha, é o que zera o `$?` para builtins no pai; explicado em `executar-comando-simples` e `ler-heredoc` 🟢
- BUG-20 e BUG-21 incorporados à §9 de `code-analysis.md` (P-8) 🟢

---

## Recomendações

- [ ] Três comportamentos travam ou derrubam o shell: o loop infinito com valor auto-referente (RX-06), o *segfault* ao falhar a abertura de um arquivo de saída (BUG-01) e o heredoc maior que ~64 KiB (RE-09). Pela resposta P-7, devem ser os primeiros a corrigir se o código for evoluído (`/reversa-forward` ou `/reversa-debugger`)
- [ ] BUG-20 (`ft_strncmp` por prefixo) é a causa comum do BUG-04 e de comandos ignorados em silêncio; uma correção na Libft afeta várias units ao mesmo tempo, então convém tratá-la como uma única feature
- [ ] Sem `minishell_tester` (L-05) nem `readline.supp` (L-06): para testes de regressão, usar os cenários Gherkin das units como oráculo e gerar supressões próprias do Valgrind

---

## Histórico de Reclassificações

| De | Para | Afirmação | Evidência |
|----|------|-----------|-----------|
| 🔴 | 🟢 | L-07: objetivo da engenharia reversa = documentar o as-is | Resposta do usuário no chat, 2026-11-22 |
| 🔴 | 🟢 | L-01: alvo = mínimo do subject; recursos extras fora do escopo | `questions.md` P-1 |
| 🔴 | 🟢 | L-03: BUG-03 (ambiente dos filhos) é bug | `questions.md` P-3 |
| 🔴 | 🟢 | L-04: `Syntax ERROR!` em stdout sem `$?=2` é bug | `questions.md` P-4 |
| 🔴 | 🟢 | P-7: crashes e travamentos devem ser corrigidos numa reimplementação | `questions.md` P-7 |
| 🟡 | 🟢 | RX-06: valor auto-referente causa loop infinito | Observado (Redator): `A='$A'` → morto pelo `timeout` |
| 🟡 | 🟢 | RE-09: heredoc maior que a capacidade do pipe trava | Observado (Revisão): 100.000 bytes → morto pelo `timeout` depois de 5 s |
| 🟡 | 🟢 | RR-08: fds de `REDI` vazam por linha | Observado (Revisão): 3 fds extras depois de 3 `cat < in1` |
| 🟡 | 🟢 | Pipe anterior a um builtin final não é fechado | Observado (Revisão): 4 fds extras depois de 2 `echo a \| pwd` |
| 🟡 | 🟢 | Token vazio chega como `""` aos comandos externos | Observado (Revisão): `ls $NOPE` → `ls: cannot access ''`, `$?=2` |
| 🟡 | 🟢 | `$?` de builtin no pai vem do filho do heredoc | Observado (Redator); `src/facade.c:51` |
| 🟢 | 🟢 (corrigido) | RR-04: o `wc` em `ls \| > o1 wc -l` **não** lê `o1` | `src/fd_handler.c:9-12` |
| — | 🟢 (novo) | BUG-20: semântica de prefixo do `ft_strncmp` | `Libft/ft_strncmp.c`; observado e gdb |
| — | 🟢 (novo) | BUG-21: `exit 2 3` encerra | `src/builtins/exit.c:51`; observado |
