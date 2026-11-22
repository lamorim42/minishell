# Lacunas Remanescentes — minishell

> Gerado pelo Revisor (Reversa) em 2026-11-22 · versão 2, depois das respostas de `questions.md`
> Todas as perguntas ao usuário foram respondidas. Não há lacunas 🔴 nas specs.

## 1. Lacunas que dependiam do usuário (todas fechadas)

| ID | Lacuna | Resposta | Efeito nas specs |
|----|--------|----------|------------------|
| L-01 | Alvo do projeto | Mínimo do subject | `${}`, `$$`, `cd -`, `-c`, tab e *word splitting* passam a "fora do escopo" 🟢 |
| L-02 | Aprovação na avaliação | Aprovado; bugs conhecidos | Registrado em `domain.md` |
| L-03 | Ambiente dos filhos | É bug | BUG-03 confirmado como defeito, documentado as-is |
| L-04 | `Syntax ERROR!` em stdout | Não deliberado; bug | BUG-12/RN-13 confirmados como defeito, documentados as-is |
| L-05 | `minishell_tester` | Resultados não guardados | Sem oráculo externo (ver §3) |
| L-06 | `readline.supp` | Não versionar | Tarefas com Valgrind precisam de um arquivo de supressão próprio (ver §3) |
| L-07 | Objetivo | Documentar o comportamento atual | ⚠️ DIVERGÊNCIAS = comportamento especificado |
| P-7 | Crashes e travamentos numa reimplementação | Corrigir | Critério de pronto das tarefas T-06 (`expandir-variaveis`), T-02 (`redirecionar-entrada-e-saida`) e T-07 (`ler-heredoc`) é a correção |
| P-8 | BUG-20/BUG-21 no `code-analysis.md` | Adicionar | Linhas novas na §9, marcadas "[Revisão 2026-11-22]" |

## 2. Itens 🟡 que permanecem inferidos

Não puderam ser confirmados pelo código nem pela execução no ambiente da revisão. Nenhum bloqueia a documentação:

| Item | Unit | Por que continua 🟡 |
|------|------|---------------------|
| RR-07: `file_exists` lido do `union` em nós de comando | `redirecionar-entrada-e-saida` | Depende do valor do ponteiro `bin`; não é determinístico |
| SG-09: builtin em filho não morre com Ctrl-C | `tratar-sinais` | Nenhum builtin bloqueia tempo suficiente para testar |
| CD-10: caminhos com mais de 500 bytes | `navegar-diretorios` | Não testado |
| Falha de `fork()` e de `pipe()` | `executar-comando-simples`, `executar-pipeline` | Exige esgotar recursos do sistema |
| `free_list` com nós de nome vazio | `executar-comando-simples` | Exige Valgrind com o readline real |
| Atalho de tamanho em `join_str_value` | `expandir-variaveis` | Correto para as entradas que o corte produz, sem prova formal |
| "Sem modelo formal de *quoting*" | `analisar-linha-de-comando`, `expandir-variaveis` | Inferido das mensagens de commit |
| Redesenho do prompt com Ctrl-C | `tratar-sinais` | O `readline` substituto dos testes não redesenha o prompt |

## 3. Limitações do ambiente de verificação

- Os testes usaram um `readline` substituto (lê linhas do stdin, sem edição nem histórico). Edição de linha, histórico e redesenho do prompt foram confirmados só pela leitura do código 🟡.
- Heredoc e sinais foram testados por pseudo-terminal (`pty`).
- O binário foi compilado com `cc -w`, fora do `Makefile`, com as mesmas fontes.
- Sem oráculo externo (L-05): os casos `@divergente` foram comparados com o comportamento conhecido do bash, não com um tester.
- Sem `readline.supp` (L-06): um teste com Valgrind vai acusar vazamentos internos do readline; é preciso gerar supressões próprias (`valgrind --gen-suppressions=all`).
