# Plano de Exploração — minishell

> Criado pelo Reversa em 2026-11-11
> Marque cada tarefa com ✅ quando concluída.
> Você pode editar este plano antes de iniciar: adicione, remova ou reordene tarefas conforme necessário.

---

## Fase 1: Reconhecimento 🔍

- [x] ✅ **Scout** — Mapeamento de estrutura de pastas e tecnologias
- [x] ✅ **Scout** — Análise de dependências e gerenciadores de pacotes
- [x] ✅ **Scout** — Identificação de entry points, CI/CD e configurações

## Decisão de organização das specs 🗂️

> Entre o Scout e o Arqueólogo, o Reversa pergunta como você quer organizar as specs (por módulo, caso de uso, endpoint, híbrida, por features ou customizada). A escolha fica persistida em `.reversa/config.toml` na seção `[specs]` e não será reperguntada em execuções futuras. Para reapresentar o menu, remova manualmente a seção.

## Fase 2: Escavação 🏗️

> O Reversa preenche esta seção com os módulos reais após o Scout concluir o reconhecimento.

- [x] ✅ **Arqueólogo** — Análise do módulo `core` (REPL, facade, init, free, arr_utils)
- [x] ✅ **Arqueólogo** — Análise do módulo `tokens` (tokenizer, léxico, sintaxe, clean_tokens)
- [x] ✅ **Arqueólogo** — Análise do módulo `exec` (expansão, lista de comandos, heredoc, PATH, fork/execve)
- [x] ✅ **Arqueólogo** — Análise do módulo `redirections` (fd_handler, <, >, >>, pipes)
- [x] ✅ **Arqueólogo** — Análise do módulo `builtins` (echo, cd, pwd, export, unset, env, exit)
- [x] ✅ **Arqueólogo** — Análise do módulo `hash_table` (ambiente em tabela hash)
- [x] ✅ **Arqueólogo** — Análise do módulo `signals` (SIGINT/SIGQUIT no prompt, pai e heredoc)
- [x] ✅ **Arqueólogo** — Análise do módulo `libft` (biblioteca de apoio, análise leve)

## Fase 3: Interpretação 🧠

- [x] ✅ **Detetive** — Arqueologia Git e ADRs retroativos
- [x] ✅ **Detetive** — Regras de negócio implícitas e máquinas de estado
- [x] ✅ **Detetive** — Matriz de permissões (RBAC/ACL)
- [x] ✅ **Arquiteto** — Diagramas C4 (Contexto, Containers, Componentes)
- [x] ✅ **Arquiteto** — ERD completo e integrações externas
- [x] ✅ **Arquiteto** — Spec Impact Matrix

## Fase 4: Geração 📝

- [x] ✅ **Redator** — Specs SDD por componente
- [x] ✅ **Redator** — OpenAPI (se aplicável) — não aplicável: o minishell não expõe API
- [x] ✅ **Redator** — User Stories (se aplicável)
- [x] ✅ **Redator** — Code/Spec Matrix

## Fase 5: Revisão ✅

- [x] ✅ **Revisor** — Revisão cruzada de specs (sem Codex)
- [x] ✅ **Revisor** — Resolução de lacunas com o usuário
- [x] ✅ **Revisor** — Relatório de confiança final

---

## Agentes Independentes

> Execute estes agentes quando os recursos estiverem disponíveis — podem rodar em qualquer fase.

- [ ] **Visor** — Análise de interface via screenshots
- [ ] **Data Master** — Análise completa do banco de dados
- [ ] **Design System** — Extração de tokens de design
- [ ] **Tracer** — Análise dinâmica (requer sistema acessível)

---

## Próximo passo

Após o Time de Descoberta concluir e o `_reversa_sdd/` estar populado, você pode disparar um dos fluxos seguintes:

- `/reversa-migrate`: orquestrador do **Time de Migração** (Paradigm Advisor → Curator → Strategist → Designer → Screen Translator → Inspector). Gera as specs do sistema novo. Saída em `_reversa_sdd/migration/` e `_reversa_sdd/screens/`.
- `/reversa-reconstructor`: gera plano bottom-up para reimplementar o software a partir das specs do legado (uma tarefa por sessão).
