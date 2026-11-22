# Perguntas para Validação — minishell

> Gerado pelo Revisor (Reversa) em 2026-11-22
> Preencha o campo **Resposta** de cada pergunta e me avise quando terminar (basta digitar `reversa`).
> Nenhuma destas perguntas bloqueia a documentação: com a decisão L-07 (documentar o comportamento atual), as specs já descrevem o que o código faz. As respostas mudam só o **rótulo** de alguns comportamentos (intencional ou defeito) e o contexto histórico.

---

## Pergunta 0 (L-07) ✅ Respondida

**Contexto:** objetivo da engenharia reversa
**Spec afetada:** todas as units
**Pergunta:** Documentar, corrigir os bugs, reescrever ou migrar?
**Impacto:** define se as ⚠️ DIVERGÊNCIAS são comportamento especificado ou defeito a corrigir.

**Resposta:** Documentar o comportamento atual (respondida no chat em 2026-11-22). Aplicada em todas as units: as ⚠️ DIVERGÊNCIAS são o comportamento especificado.

---

## Pergunta 1 (L-01) ✅ Respondida

**Contexto:** escopo do projeto; vários comportamentos dependem do alvo (modo não interativo ausente em `src/main.c`, tab não separa tokens em `src/tokens/count_tks.c`, falta de `${}`/`$$`/*word splitting* em `src/exec/expand_var.c`)
**Spec afetada:** [`reversa-sdd/iniciar-sessao-e-ler-comando/design.md`], [`reversa-sdd/analisar-linha-de-comando/requirements.md`], [`reversa-sdd/expandir-variaveis/design.md`], `user-stories/*.md`
**Pergunta:** Qual versão do *subject* do minishell da 42 o projeto seguiu? O alvo era "igual ao bash" ou só o mínimo exigido pelo subject?
**Impacto:** se o alvo era o subject, itens como `${}`, `$$`, tab, `cd -` e modo `-c` passam de ⚠️ DIVERGÊNCIA para "fora do escopo" (🟢). Se era o bash, continuam como divergências conhecidas.

**Resposta:** minímo exigido, por enquanto vai ficar fora do escopo

---

## Pergunta 2 (L-02) ✅ Respondida

**Contexto:** histórico do projeto; os bugs da §9 de `code-analysis.md` e os novos BUG-20/BUG-21
**Spec afetada:** [`reversa-sdd/confidence-report.md`], [`reversa-sdd/domain.md`]
**Pergunta:** O projeto foi aprovado na avaliação da 42? Com qual nota? Algum dos bugs documentados foi apontado pelos avaliadores?
**Impacto:** indica se os bugs eram conhecidos ou aceitos; ajuda a separar "decisão consciente" de "defeito não detectado" no histórico.

**Resposta:** foi aprovado, bugs são conhecidos

---

## Pergunta 3 (L-03) ✅ Respondida

**Contexto:** `execve(list->bin, list->args, line->envp)` em `src/exec/exec_path.c:34` usa o `envp` original de `main`, não a tabela hash
**Spec afetada:** [`reversa-sdd/gerenciar-variaveis-de-ambiente/requirements.md`], [`reversa-sdd/executar-comando-simples/requirements.md`] (RE-03 / BUG-03)
**Pergunta:** `export` e `unset` não chegarem aos programas filhos (`export ZZ=1; sh -c 'echo $ZZ'` imprime vazio) foi intencional ou um bug?
**Impacto:** se intencional, BUG-03 vira regra de negócio (🟢, sem ⚠️). Se bug, continua como defeito documentado.

**Resposta:** é um, bug

---

## Pergunta 4 (L-04) ✅ Respondida

**Contexto:** `free_sintax` em `src/free.c:47-56` usa `printf("Syntax ERROR!\n")` (stdout) e não grava `$?`
**Spec afetada:** [`reversa-sdd/analisar-linha-de-comando/design.md`], [`reversa-sdd/analisar-linha-de-comando/tasks.md`], [`reversa-sdd/iniciar-sessao-e-ler-comando/tasks.md`]
**Pergunta:** A mensagem `Syntax ERROR!` em stdout, sem `$?=2`, foi uma escolha deliberada?
**Impacto:** muda o rótulo de RN-13 e BUG-12 (decisão de projeto ou defeito).

**Resposta:** não foi deliberado e bug também

---

## Pergunta 5 (L-05) ✅ Respondida

**Contexto:** o histórico Git cita um `minishell_tester` usado durante o desenvolvimento, que não está no repositório
**Spec afetada:** [`reversa-sdd/confidence-report.md`] (seção de recomendações de teste)
**Pergunta:** Qual era o `minishell_tester` usado (repositório/URL)? Os resultados dele estão guardados em algum lugar?
**Impacto:** permite usar esse tester como oráculo adicional e confirmar os casos `@divergente` de forma independente.

**Resposta:** os resultados não estão guardados

---

## Pergunta 6 (L-06) ✅ Respondida

**Contexto:** o alvo `val` do `Makefile` referencia um `readline.supp` (supressões do Valgrind) que não está versionado
**Spec afetada:** [`reversa-sdd/dependencies.md`], tarefas de Valgrind nas units (`TT-*`)
**Pergunta:** Vale versionar o `readline.supp`? Você ainda tem esse arquivo?
**Impacto:** sem ele, as tarefas de teste com Valgrind mostram vazamentos do readline que não são do projeto.

**Resposta:** não

---

## Pergunta 7 (nova) ✅ Respondida

**Contexto:** três comportamentos documentados travam ou derrubam o shell: loop infinito com valor auto-referente (RX-06, `src/exec/expand_var.c:27-28`), *segfault* ao falhar a abertura de arquivo de saída (BUG-01, `src/fd_handler_utils.c:39-43`) e travamento com heredoc maior que ~64 KiB (RE-09, `src/exec/here_doc_utils.c:34-40`)
**Spec afetada:** [`reversa-sdd/expandir-variaveis/tasks.md`] T-06, [`reversa-sdd/redirecionar-entrada-e-saida/tasks.md`] T-02, [`reversa-sdd/ler-heredoc/tasks.md`] T-07
**Pergunta:** Se um dia houver uma reimplementação a partir destas specs, esses três casos devem ser reproduzidos (fidelidade total) ou corrigidos (as tarefas já descrevem a correção)?
**Impacto:** hoje as três tarefas descrevem a correção e marcam o as-is como documentado. Se a resposta for "reproduzir", as tarefas passam a ter o as-is como critério de pronto.

**Resposta:** corrigidos

---

## Pergunta 8 (nova) ✅ Respondida

**Contexto:** o Redator encontrou falhas que não estão na §9 de `code-analysis.md`: BUG-20 (`ft_strncmp` com semântica de prefixo faz comandos como `P`, `R x` e `$NOPE ls` serem ignorados) e BUG-21 (`exit 2 3` encerra o shell)
**Spec afetada:** [`reversa-sdd/code-analysis.md`] §9 (artefato do Arqueólogo), [`reversa-sdd/traceability/code-spec-matrix.md`] §5
**Pergunta:** Quer que eu acrescente BUG-20 e BUG-21 à lista de bugs da §9 de `code-analysis.md`, como adendo marcado "[Revisão 2026-11-22]"? Hoje eles só estão nas units e na code-spec matrix.
**Impacto:** centraliza a lista de bugs em um só lugar. Sem isso, quem lê só o `code-analysis.md` não vê os dois achados.

**Resposta:**  adicionar

---

## Pergunta 9 (L-08) ✅ Respondida

**Contexto:** unificação da `featura/reversa` com a `feature/refac-rust` (PR #36). As specs descrevem o comportamento atual da versão em C (L-07), incluindo as ⚠️ DIVERGÊNCIAS em relação ao bash. A reescrita em Rust segue o POSIX.1-2024 (capítulo 2, *Shell Command Language*).
**Spec afetada:** todas as units, nos itens marcados ⚠️ DIVERGÊNCIA ou `@divergente`; [`reversa-sdd/traceability/code-spec-matrix.md`] §6
**Pergunta:** Na reescrita em Rust, os casos em que o C diverge do bash/POSIX devem ser reproduzidos (fidelidade ao C) ou seguir o POSIX?
**Impacto:** define o critério de pronto da reescrita. Com "POSIX", cada ⚠️ DIVERGÊNCIA vira um comportamento que o Rust **não** reproduz, e o teste de aceitação passa a comparar com o POSIX (e com `bash --posix`/`dash` como referência prática), não com o binário em C.

**Resposta:** seguir o POSIX (respondida no chat em 2026-11-22). Regras de aplicação:

1. **Versão em C:** nada muda. L-07 continua valendo e as specs seguem descrevendo o que o C faz.
2. **Versão em Rust:** onde a spec marca ⚠️ DIVERGÊNCIA ou `@divergente`, vale o POSIX. O comportamento do C fica só como registro histórico, não como critério de pronto.
3. **Bugs (BUG-xx, RX-xx):** não são reproduzidos no Rust (consistente com a Pergunta 7, "corrigidos").
4. **Escopo (Pergunta 1):** a lista "fora do escopo" (`${}`, `$$`, modo `-c`, `cd -` etc.) vale para o C. No Rust, o que já está implementado segue o POSIX. O que ainda não está implementado entra no escopo pelos marcos M3/M4 e pelos documentos de planejamento da refatoração.
5. **Em caso de dúvida:** consultar o PRD, o TDD e os ADRs da refatoração em Rust, que têm precedência sobre estas specs para a versão em Rust. Atenção: a numeração dos ADRs da refatoração (citados no código como ADR-003, ADR-004, ADR-007, ADR-010, ADR-013 a ADR-015) **não** corresponde à dos ADRs desta extração em `reversa-sdd/adrs/` (ADR-001 a ADR-010, que descrevem a versão em C).
