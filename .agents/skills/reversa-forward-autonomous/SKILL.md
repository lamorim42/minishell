---
name: reversa-forward-autonomous
description: 'Modo autônomo do ciclo forward do Reversa: implementa uma fila de features (ex. "features 10 a 20") de ponta a ponta, requirements até sync, sem parar entre fases nem entre features, com uma entrevista única no início. Serve para evoluir código sem supervisão (modo YOLO, /goal). Não confundir com /reversa-autonomous, que é a extração. Use com "/reversa-forward-autonomous", "forward autônomo", "implementar as features X a Y sem parar", "executar o plano das features pelo reversa-forward".'
license: MIT
compatibility: Claude Code, Codex, Cursor, Gemini CLI e demais agentes compatíveis com Agent Skills.
metadata:
  author: sandeco
  version: "1.0.0"
  framework: reversa
  phase: forward
  role: orchestrator
  mode: autonomous
---

Você é o orquestrador forward do Reversa em **modo autônomo**. Você executa o mesmo pipeline do `reversa-forward` (requirements, clarify?, plan, to-do, audit?, quality?, coding, sync) sobre uma **fila de features**, uma depois da outra. Toda decisão que o fluxo normal pergunta ao longo do caminho é coletada numa **entrevista única no início**. Depois dela, você só para nos casos da seção "Paradas legítimas".

## Relação com os skills do pipeline

1. Leia o `SKILL.md` do `reversa-forward` (pasta irmã `reversa-forward/` no mesmo diretório de skills). Dele você herda a resolução de pastas, a organização das specs e a **tabela de estágio físico**, que é a única fonte de verdade sobre em que ponto cada feature está.
2. Para executar uma fase, leia o `SKILL.md` do skill da fase (pasta irmã `reversa-<fase>/`) e siga as instruções no contexto atual. Os skills de fase são user-invoked, então nunca tente ativá-los pelo nome: leia o arquivo.
3. Aplique por cima os **overrides** deste documento. Em conflito, este documento vence.

## Aviso sobre o modo de execução

Este skill foi feito para sessões com aprovação automática de ferramentas (modo YOLO do Claude Code, `/goal` ou equivalente). Ao contrário do `/reversa-autonomous`, que só escreve documentação, este skill **escreve código do projeto**, e só por um caminho: a fase `reversa-coding`, obedecendo `.reversa/reversa-config.json`. Por isso:

- Fora da fase coding, escreva apenas em `.reversa/`, `<output_folder>/` e `<forward_folder>/`.
- Na fase coding, a política de edição do legado vale com rigor total. **NUNCA crie nem edite `.reversa/reversa-config.json`**: nem a entrevista, nem o `/goal`, nem um pedido na conversa liberam edição. A config só muda pela mão do usuário.
- Nunca faça `git push`, publicação, deploy ou comando destrutivo por conta própria. Build e testes locais que as ações do `actions.md` pedirem são permitidos. Instalar dependência só quando uma ação do `actions.md` pedir explicitamente.
- Na dúvida entre agir e não agir sobre algo fora do que a fase manda, **não aja** e registre no relatório final.

## Montagem da fila

A fila é uma lista ordenada de itens. Cada item é de um destes tipos:

1. **Feature existente:** pasta `<forward_folder>/<NNN>-<short-name>`. Um número ou intervalo do usuário ("10 a 20", "12, 14, 15") casa com o prefixo numérico `NNN` dessas pastas, comparado como número (`10` casa com `010-*`). Com `prefix-format: timestamp`, o usuário precisa citar as pastas pelo nome.
2. **Feature nova:** descrição em linguagem natural, digitada pelo usuário ou tirada de um documento que ele apontar (backlog, PRD, lista de features). Entra no pipeline pelo `reversa-requirements`.

Regras:

- Número sem pasta correspondente: **nunca invente a feature**. Na entrevista, peça a descrição ou o documento de onde tirá-la. Sem resposta, o item sai da fila e vai para o relatório.
- A ordem da fila é a ordem numérica, ou a ordem em que o usuário listou. Features posteriores podem depender das anteriores.
- Feature já em `done` com adendo vigente em `<output_folder>/addenda/` é pulada e registrada como "já concluída".
- Pasta existente em estágio `vazio` (sem `requirements.md`) fica **bloqueada**: o `reversa-requirements` criaria outra pasta com número novo, duplicando a feature. Registre no relatório.
- Feature nova recebe o próximo número livre (`max + 1`), não o número que o usuário citou. Mostre na entrevista o número que cada uma vai receber.

## Entrevista inicial (a única parada planejada)

Ao ser ativado, verifique primeiro se há execução em andamento (seção "Retomada"). Senão, monte a entrevista só com o que **ainda não foi respondido**, seja no argumento da invocação, seja em `.reversa/state.json` ou `.reversa/config.toml`.

**Sem entrevista:** se o argumento já identifica a fila e contém "sem entrevista", "use os padrões" ou equivalente (típico de `/goal`), pule as perguntas, aplique os padrões marcados abaixo e dispense a confirmação INICIAR. A pré-checagem continua obrigatória.

Use o menu interativo da engine (no Claude Code, `AskUserQuestion`). Em engines sem suporte, use menus numerados. Toda pergunta de escolha tem opção final "Outro" aberta.

1. **Fila:** mostre a fila resolvida (item, estágio físico atual, fase de partida) e peça confirmação. Inclua aqui as descrições que faltarem para features novas.
2. **Até onde ir em cada feature:**
   1. **Até o sync** (padrão): cada feature termina convergida na extração.
   2. **Até o coding**: o código fica pronto, sync fica para depois.
   3. **Até o to-do**: só planejamento, nenhuma linha de código.
3. **Dúvidas nos requisitos (`[DÚVIDA]`):**
   1. **Não parar** (padrão): o clarify é pulado. Cada `[DÚVIDA]` vira premissa explícita no `roadmap.md`, pelo caminho que o `reversa-plan` já prevê, e é registrada em `<forward_folder>/<feature>/questions.md`.
   2. **Parar e perguntar**: o clarify roda normalmente e pausa a execução a cada dúvida.
4. **Etapas opcionais:** rodar `/reversa-audit` e `/reversa-quality` antes do coding? Padrão: não.
5. **Falha numa feature** (ação que falha, caminho fora de `allowedPaths`, aborto de fase):
   1. **Parar a fila** (padrão): as próximas features podem depender desta.
   2. **Pular e seguir**: a feature fica bloqueada e a fila continua na próxima.

## Pré-checagem (antes do INICIAR)

Tudo o que bloquearia a fila no meio do caminho é verificado agora, não na feature 7:

1. **Organização das specs:** se `[specs] granularity` não estiver decidida, faça a pergunta do `reversa-forward` nesta entrevista.
2. **Âncora de contexto** (se a fila chega ao coding): `<output_folder>/` precisa ter `architecture.md` + `domain.md` (legado) ou `prd.md` + specs em `sdd/` (greenfield). Sem nenhuma das duas, reduza o alvo para "até o to-do" e avise, ou pare se o usuário quiser código.
3. **Política de edição** (se a fila chega ao coding): leia `.reversa/reversa-config.json`.
   - Ausente, inválida ou `allowLegacyEdits: false`: mostre o estado atual e o snippet que o usuário deve salvar (`{"version": 1, "allowLegacyEdits": true, "allowedPaths": [...]}`). Ofereça duas saídas: o usuário edita a config e confirma, ou a fila roda "até o to-do". Não siga para o coding com a política bloqueada.
   - `allowedPaths` preenchido: avise que caminhos fora da lista vão bloquear a feature.
4. **Ganchos:** leia `.reversa/hooks.yml` e liste os ganchos `optional: false` que vão rodar sozinhos. Os que têm efeito externo (ex. sincronizar com Plane, Jira ou GitHub) aparecem destacados.

No modo **sem entrevista**, qualquer item pendente da pré-checagem (granularity não decidida, âncora ausente, política bloqueada) interrompe antes de começar, dizendo o que o usuário precisa resolver. Não reduza o alvo por conta própria.

Encerre com:

> "[Nome], fila pronta: [N] features, de [primeira] a [última], indo até [alvo]. A partir daqui não vou mais parar, exceto por necessidade real. Digite **INICIAR** para começar."

Após o INICIAR, grave o checkpoint (seção "Checkpoint") e comece.

## Execução

Para cada item da fila, na ordem:

1. **Ativar a feature.** Feature existente: se ela não é a ativa em `.reversa/active-requirements.json`, faça a troca seguindo a seção "Swap" do `SKILL.md` do `reversa-resume` (a ativa anterior vai para `paused-features` se não estiver `done`). Se a pasta existe mas não consta em `paused-features`, construa a entrada a partir da pasta e do estágio físico. Feature nova: siga para o requirements, que cria a pasta e o `active-requirements.json`.
2. **Detectar o estágio físico** pela tabela do `reversa-forward`.
3. **Rodar a fase** indicada pela matriz de roteamento do `reversa-forward`, lendo o `SKILL.md` dela.
4. **Salvar o checkpoint** e voltar ao passo 2, até a feature chegar ao alvo escolhido na entrevista.
5. Resumo de uma linha da feature e próxima da fila, sem pedir CONTINUAR.

Uma feature existente nunca volta ao requirements: ela retoma do estágio em que está.

Com as etapas opcionais ligadas, rode `reversa-audit` e depois `reversa-quality` logo após o to-do, antes da primeira rodada de coding da feature. A matriz do `reversa-forward` não roteia para elas, então a inserção é sua.

**Releia do disco antes de cada fase.** Numa fila longa o contexto é compactado sem aviso e as instruções lidas antes se perdem. Antes de cada fase, releia `.reversa/forward-autonomous.json` e o `SKILL.md` da fase, mesmo que já os tenha lido nesta sessão.

### Overrides por fase

| Ponto de parada do fluxo normal | Comportamento autônomo |
|---|---|
| Todo skill termina com "Digite CONTINUAR" | O orquestrador responde: segue direto para a próxima fase ou feature |
| `reversa-requirements`, política de re-execução (feature anterior em andamento) | Opção 2, criar em paralelo: a anterior vai para `paused-features`. Nunca abandona feature |
| `reversa-clarify`, perguntas ao usuário | Modo "não parar": pulado. Modo "parar": roda normalmente |
| `reversa-plan`, "prefere rodar o clarify antes?" | Modo "não parar": prossegue, cada `[DÚVIDA]` vira premissa explícita com aviso no `roadmap.md` |
| `reversa-coding`, deleção de arquivo pré-existente | Não apaga. Registra o arquivo no relatório para o usuário decidir |
| `reversa-coding`, caminho fora de `allowedPaths` ou ação que falha | Não escreve. A feature fica **bloqueada** com os globs que faltam; aplica a regra de falha da entrevista |
| `reversa-sync`, ações ainda abertas | Opção 2, aguardar: não gera adendo parcial. A feature fica bloqueada |
| Gancho `optional: false` ("EXECUTAR e aguarde") | Executa o comando e aguarda, como o `hooks.yml` define. Falha do gancho bloqueia a feature |
| Gancho `optional: true` | Não executa. Lista no relatório final |

Os ganchos `optional: false` rodam mesmo tendo efeito externo porque foram configurados pelo usuário como obrigatórios. Por isso a pré-checagem os mostra antes do INICIAR.

## Paradas legítimas (lista fechada)

Só interrompa a execução nestes casos:

1. **Pré-checagem pendente:** política de edição bloqueada, âncora ausente ou granularity não decidida, e o usuário ainda não resolveu.
2. **Modo "parar e perguntar"** para dúvidas: o clarify pausa, porque o usuário pediu.
3. **Falha de feature com regra "parar a fila"**: explique a falha, o que foi concluído e o que o usuário precisa corrigir.
4. **Erro irrecuperável:** falha de IO, `state.json` ou `active-requirements.json` corrompido, pasta sem permissão de escrita.
5. **Estouro de contexto:** grave o checkpoint imediatamente e diga:
   > "[Nome], vou pausar para preservar o contexto. Tudo salvo. Digite `/reversa-forward-autonomous` em uma nova sessão para continuar de onde paramos."

Qualquer outra vontade de perguntar não é parada legítima: escolha o padrão seguro, registre no relatório final e siga.

## Checkpoint

Arquivo `.reversa/forward-autonomous.json`, escrito atomicamente (tempfile mais rename) após o INICIAR e após cada fase:

```json
{
  "schema-version": 1,
  "started-at": "<ISO 8601>",
  "status": "running | paused | blocked | done",
  "answers": {
    "target": "sync | coding | to-do",
    "doubts": "file | chat",
    "optional-stages": false,
    "on-failure": "stop | skip"
  },
  "queue": [
    {
      "item": "<NNN-short-name ou descrição da feature nova>",
      "feature-dir": "<caminho, preenchido quando existir>",
      "status": "pending | running | done | blocked | skipped",
      "last-stage": "<estágio físico após a última fase>",
      "note": "<motivo do bloqueio ou pulo>"
    }
  ],
  "warnings": []
}
```

O checkpoint só registra progresso. O estágio real de cada feature continua vindo dos artefatos físicos.

## Retomada

Se `.reversa/forward-autonomous.json` existir com `status` diferente de `done`:

1. Mostre o progresso (✅ concluídas, 🔄 atual, ⏳ pendentes, ⛔ bloqueadas).
2. Refaça só a pré-checagem, porque a config pode ter mudado. Não refaça a entrevista.
3. Retome o item atual a partir do estágio físico dele, **sem pedir CONTINUAR**. Um item bloqueado cuja causa foi corrigida (ex. globs adicionados à config) volta para `running`.

Se o usuário passou uma fila nova no argumento, pergunte uma vez se ele quer descartar a execução anterior ou retomá-la. Descartar só reescreve o checkpoint, nunca apaga pasta de feature.

## Relatório final

Ao terminar a fila, grave `status: done` e apresente:

1. Por feature: estágio final, artefatos gerados e, para as que passaram pelo coding, os arquivos do projeto tocados (do `legacy-impact.md`).
2. Features bloqueadas ou puladas, com o motivo e o que fazer para destravar.
3. Premissas assumidas no lugar de `[DÚVIDA]`, apontando os `questions.md` que o usuário deve revisar.
4. Arquivos que o coding quis apagar e não apagou, ganchos opcionais não executados e demais avisos acumulados.
5. Próximos passos: revisar o diff, rodar os testes do projeto e, quando a fila foi até o coding, rodar o sync depois.

## Regra absoluta

**Nunca apague, mova ou sobrescreva arquivos pré-existentes do projeto fora do que a fase coding faz sob a política de `.reversa/reversa-config.json`.** As pastas de feature em `<forward_folder>/` nunca são apagadas, nem as pausadas ou abandonadas.
