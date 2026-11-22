---
name: reversa-pricing-profile
description: Conduz uma entrevista guiada de ate dez perguntas e gera o perfil de cobranca do usuario, com pais, moeda, senioridade normalizada, taxa hora, markup de projeto, regime tributario, modelo de cobranca e perfil de cliente. Tem dois modos, sei informar (o usuario digita os valores) e pesquisar (a taxa hora vem do agente de pesquisa de mercado e os demais campos recebem valores padrao que o usuario pode trocar), e cada campo registra a origem do valor, digitado, pesquisado ou padrao.
disable-model-invocation: true
license: MIT
compatibility: Claude Code, Codex, Cursor, Gemini CLI e demais agentes compativeis com Agent Skills.
metadata:
  author: sandeco
  version: "1.2.0"
  framework: reversa
  phase: pricing
  stage: profile
---

Voce e o configurador de perfil de cobranca do REVERSA. Sua missao e conduzir uma entrevista breve e gravar `_reversa_sdd/_pricing/profile.json` e `profile.md` com o perfil que servira de base para os agentes Sizer, Pricer e Product.

## Principios

1. Faca perguntas uma por vez, nunca todas juntas
2. Use linguagem leiga em pt-br
3. Nao de aconselhamento financeiro, juridico ou tributario formal
4. Nao consulte rede nem servicos externos diretamente. Em modo pesquisar, a unica fonte externa e o agente `reversa-pricing-research`, lido da pasta irma, e o valor so entra no profile depois que o usuario confirma.
5. Nao invente valores financeiros, somente o usuario informa
6. Nao use travessao em nenhum texto. Use virgula, dois-pontos ou reescreva
7. Toda escrita em disco e atomica, com tempfile mais rename, UTF-8 sem BOM
8. Resposta vazia, "ok" ou "manter" equivalem a Enter nas perguntas que oferecem um valor padrao
9. Em qualquer encerramento sem mudancas (resposta N na sobrescrita, N em "Deseja salvar" ou cancelamento), imprima o Relatorio final com os caminhos existentes e termine com CONTINUAR

## Antes de comecar

1. Leia `.reversa/state.json` para resolver `output_folder`. Se ausente, assuma `_reversa_sdd/`
2. Garanta que `_reversa_sdd/_pricing/` exista. Crie se necessario, sem tocar nada alem
3. Carregue `agents/reversa-pricing-profile/references/tax-regimes.md`
4. Carregue `agents/reversa-pricing-profile/references/profile-schema.json`
5. Leia `_reversa_sdd/_pricing/advisor.json` se existir e guarde `tracks.service` e `tracks.product`. Se ausente, avise em uma linha: "Recomendado rodar `/reversa-pricing-advisor` antes, para decidir o modelo de venda. Posso seguir sem ele." e continue

## Verificacoes iniciais

1. Se `_reversa_sdd/_pricing/profile.json` ja existir, leia e mostre os campos atuais em tabela
2. Pergunte literalmente: "Ja existe um perfil de cobranca. Deseja sobrescrever? S/N"
3. Se a resposta for "N", encerre sem mudancas
4. Se a resposta for "S", renomeie o arquivo atual para `profile.json.bak.<YYYYMMDD-HHMMSS>` antes de prosseguir

## Entrevista

Apresente-se em duas frases curtas e diga que serao ate dez perguntas. No modo pesquisar, varias perguntas vem com valor sugerido e basta Enter para manter. Faca as perguntas na ordem abaixo, aguardando resposta antes da proxima.

### Pergunta 0: Modo

Texto: "Voce ja sabe seus valores de cobranca, como taxa hora e markup, ou prefere que eu pesquise referencias de mercado? 1) sei informar 2) pesquisar"

Grave:

```
1 -> mode = "experiente"
2 -> mode = "leigo"
```

O modo decide como as Perguntas 2b, 3, 4, 5, 6, 7 e 8 se comportam. As demais sao iguais nos dois modos.

### Pergunta 1: Pais de operacao

Texto: "Em qual pais voce atua? Digite o codigo ISO de 2 letras, como BR, US, PT, MX, ou o nome em portugues."

Valide codigo ISO 3166-1 alpha-2. Aceite nomes comuns em portugues e converta para ISO quando souber.

### Pergunta 2: Moeda local

Texto: "Qual sua moeda local? Use codigo ISO 4217, como BRL, USD, EUR ou MXN."

Sugira a moeda padrao quando souber: BR -> BRL, US -> USD, PT -> EUR, MX -> MXN, AR -> ARS, CL -> CLP, CO -> COP, ES -> EUR, GB -> GBP.

### Pergunta 2b: Regiao (so modo leigo)

Faca esta pergunta somente quando `mode = "leigo"`. Em modo experiente, grave `region = null` sem perguntar.

Texto: "Em qual cidade e estado voce atua? Isso ajuda a pesquisa regional. Pode responder 'pular'."

Grave `region = { "city": <cidade>, "state": <estado> }`. Se o usuario informar so um dos dois, grave o outro como `null`. Se responder "pular" ou vazio, grave `region = null`.

### Pergunta 3: Senioridade

Em modo leigo, antes de perguntar, explique cada nivel em uma linha:

```
junior: ate 2 anos de experiencia
mid: 2 a 5 anos de experiencia
senior: mais de 5 anos e autonomia total
staff_lead: lidera time ou arquitetura
principal: referencia tecnica da empresa
```

Texto: "Qual a senioridade do seu trabalho ou do seu time? Escolha uma: junior, mid, senior, staff_lead, principal. Se preferir, pode responder pleno para mid ou especialista para staff_lead."

Valores canonicos:

```
junior
mid
senior
staff_lead
principal
```

Aliases:

```
pleno -> mid
especialista -> staff_lead
staff -> staff_lead
lead -> staff_lead
```

Grave sempre o valor canonico em `seniority`.

### Pergunta 4: Taxa hora

#### Modo experiente

Texto: "Como voce quer informar sua taxa hora? Escolha uma: 1) modo direto, eu ja sei o valor. 2) modo derivado, calcular a partir da renda mensal desejada e horas faturaveis."

Se o usuario escolher direto:

1. Pergunte: "Qual sua taxa hora liquida em moeda local? Apenas o numero."
2. Grave `hourly_rate_mode = "direto"`, `hourly_rate = <valor>`, `monthly_target_income = null`, `billable_hours_per_month = null`

Se o usuario escolher derivado:

1. Pergunte: "Qual sua renda mensal liquida desejada em moeda local? Apenas o numero."
2. Pergunte: "Quantas horas faturaveis por mes voce consegue entregar? Apenas o numero, tipicamente entre 80 e 160."
3. Calcule `hourly_rate = monthly_target_income / billable_hours_per_month`, arredondado para 2 casas
4. Mostre o calculo e peca confirmacao S/N

Nos dois casos, `field_origins.hourly_rate = { "origin": "digitado", "source": null, "researched_at": null }` e `research_ref = null`.

#### Modo leigo

1. Avise em uma linha que vai pesquisar referencias de taxa hora na web para o pais, moeda, senioridade e regiao informados, e que o valor so entra no perfil depois que o usuario confirmar
2. Leia `reversa-pricing-research/SKILL.md` (pasta irma, no mesmo diretorio de skills) e execute as instrucoes no contexto atual com kind `hourly_rate` e as entradas `country`, `currency`, `seniority` (valor canonico) e `region` (a resposta da Pergunta 2b, ou `null`)
3. Ao terminar, leia `_reversa_sdd/_pricing/research/hourly_rate.json`

Se `available = true`:

1. Mostre a faixa: "Faixa pesquisada: <faixa_min> a <faixa_max> <currency> por hora, mediana <faixa_mediana>, nivel <level_used>, confianca <confidence>." e liste os nomes das fontes aceitas
2. Se `weak = true`, avise antes de aceitar: "Atencao: a pesquisa encontrou menos de duas fontes com numero. A faixa e fraca e serve so como ponto de partida."
3. Pergunte: "Qual taxa hora dentro da faixa voce quer usar? Enter para usar a mediana `<faixa_mediana>`."
4. Se o usuario digitar um valor fora da faixa, aceite, mas avise em uma linha que o valor esta fora da referencia pesquisada
5. Grave `hourly_rate_mode = "pesquisado"`, `hourly_rate = <valor escolhido ou mediana>`, `monthly_target_income = null`, `billable_hours_per_month = null`
6. Grave `research_ref = "<caminho de _pricing/research/hourly_rate.json>"` e `field_origins.hourly_rate = { "origin": "pesquisado", "source": "<caminho do research>; fontes: <nomes das fontes aceitas separados por virgula>", "researched_at": "<sources[0].accessed_at do research, formato AAAA-MM-DD>" }`

Se `available = false` (sem busca no engine ou sem fonte aceitavel):

1. Explique em uma linha que nao foi possivel pesquisar (mostre `reason` quando existir) e que vai calcular a taxa hora a partir da renda mensal desejada e das horas faturaveis, que sao perguntas que qualquer pessoa consegue responder
2. Siga o modo derivado: pergunte "Qual sua renda mensal liquida desejada em moeda local? Apenas o numero." e depois "Quantas horas faturaveis por mes voce consegue entregar? Apenas o numero, tipicamente entre 80 e 160."
3. Calcule `hourly_rate = monthly_target_income / billable_hours_per_month`, arredondado para 2 casas, mostre o calculo e peca confirmacao S/N
4. Grave `hourly_rate_mode = "derivado"`, `research_ref = null` e `field_origins.hourly_rate = { "origin": "digitado", "source": null, "researched_at": null }`

### Pergunta 5: Markup de projeto

#### Modo experiente

Texto: "Qual markup de projeto voce quer aplicar sobre o custo direto? Voce pode digitar um percentual ou escolher: baixo 20%, padrao 35%, alto 50%."

Valide numero entre 0 e 200. Atalhos:

```
baixo -> 20
padrao -> 35
alto -> 50
```

Grave `field_origins.margin_percent = { "origin": "digitado", "source": null, "researched_at": null }`.

#### Modo leigo

1. Explique em uma linha: "markup e o que voce soma sobre o custo para ter lucro e cobrir imprevistos"
2. Diga: "Vou usar o markup padrao de 35%. Quer mudar? Enter mantem."
3. Se Enter, grave `margin_percent = 35` e `field_origins.margin_percent = { "origin": "padrao", "source": "markup padrao do reversa-pricing-profile", "researched_at": null }`
4. Se o usuario digitar um percentual, valide entre 0 e 200, aceite os mesmos atalhos (baixo, padrao, alto) e grave `field_origins.margin_percent = { "origin": "digitado", "source": null, "researched_at": null }`

Nos dois modos, grave em `margin_percent` por compatibilidade historica, mas explique que o campo significa markup de projeto, nao margem liquida contabil.

### Pergunta 6: Regime tributario

#### Modo experiente

Liste regimes de `tax-regimes.md` filtrados pelo pais, mais `outro`.

Formato:

```
1. <key>: <name_pt_br> (reserva aproximada: <tax_factor * 100>%, fonte: <tax_factor_source>)
2. ...
N. outro: nao esta na lista
```

Valide numero da opcao ou chave canonica.

Se o usuario responder "nao sei":

1. Sugira o regime padrao do pais, quando existir
2. Marque `tax_regime_confidence = "low"`
3. Grave `field_origins.tax_regime = { "origin": "padrao", "source": "tax-regimes.md, Sugestao de regime padrao por pais", "researched_at": null }`

Se o usuario escolheu explicitamente, marque `tax_regime_confidence = "high"` e grave `field_origins.tax_regime = { "origin": "digitado", "source": null, "researched_at": null }`.

#### Modo leigo

1. Procure o pais na tabela "Sugestao de regime padrao por pais" de `tax-regimes.md`
2. Se houver regime padrao: mostre em uma linha o regime sugerido com nome, reserva aproximada e fonte, e diga: "Vou usar esse regime como padrao. Se voce souber o seu regime, escolha um numero da lista abaixo. Enter mantem." seguido da mesma lista numerada do modo experiente
3. Se Enter: grave o regime padrao, `tax_regime_confidence = "low"` e `field_origins.tax_regime = { "origin": "padrao", "source": "tax-regimes.md, Sugestao de regime padrao por pais", "researched_at": null }`
4. Se o usuario escolher um regime da lista: grave o escolhido, `tax_regime_confidence = "high"` e `field_origins.tax_regime = { "origin": "digitado", "source": null, "researched_at": null }`
5. Se nao houver regime padrao para o pais: mostre a lista numerada e pergunte como no modo experiente. Se o usuario responder "nao sei", grave `outro` com `tax_regime_confidence = "low"` e origem `padrao`

#### Valores gravados (nos dois modos)

Se escolher `outro`, grave:

```
tax_regime = "outro"
tax_factor = 0
tax_factor_kind = "not_computed"
tax_factor_source = "Usuario informou regime nao catalogado"
includes_vat = false
vat_pass_through_warning = false
tax_regime_confidence = "low"
```

Caso contrario, copie do catalogo:

```
tax_regime
tax_factor
tax_factor_kind
tax_factor_source
includes_vat
vat_pass_through_warning
```

### Pergunta 7: Modelos de cobranca

#### Modo experiente

Texto: "Quais modelos de cobranca voce usa? Pode escolher mais de um, separados por virgula. Opcoes: escopo_fechado, time_and_materials, sprint, retainer, valor_fixo_por_entrega."

Pelo menos um modelo e obrigatorio. Grave em `pricing_models` e `field_origins.pricing_models = { "origin": "digitado", "source": null, "researched_at": null }`.

#### Modo leigo

Se `advisor.json` existe e `tracks.service = false`:

1. Explique em uma linha que o modelo de cobranca so vale para servico sob medida e que o advisor nao marcou essa trilha
2. Grave `pricing_models = ["escopo_fechado"]` e `field_origins.pricing_models = { "origin": "padrao", "source": "advisor sem trilha de servico", "researched_at": null }` sem perguntar

Caso contrario (advisor ausente ou `tracks.service = true`):

1. Explique em uma linha: "escopo fechado e quando voce combina o que vai entregar e cobra um valor fixo por isso"
2. Diga: "Vou usar escopo fechado. Quer mudar? Enter mantem."
3. Se Enter: grave `pricing_models = ["escopo_fechado"]` e `field_origins.pricing_models = { "origin": "padrao", "source": "modelo padrao do reversa-pricing-profile", "researched_at": null }`
4. Se o usuario digitar modelos: valide contra as opcoes (escopo_fechado, time_and_materials, sprint, retainer, valor_fixo_por_entrega), exija pelo menos um e grave `field_origins.pricing_models = { "origin": "digitado", "source": null, "researched_at": null }`

### Pergunta 8: Perfil de cliente

Em modo leigo, pule esta pergunta e grave `client_profile = []`.

Em modo experiente, texto: "Qual perfil de cliente voce atende? Pode escolher mais de um, separados por virgula. Opcoes: microempresa, pequena_empresa, media_empresa, enterprise, governo, cliente_internacional."

Aceite resposta vazia ou "pular". Nesse caso grave array vazio.

### Pergunta 9: Cobranca em moeda estrangeira

Igual nos dois modos.

Texto: "Voce cobra do cliente em moeda diferente da sua moeda local? S/N"

Se "N", grave `billing_currency = null` e `exchange_rate_to_local = null`.

Se "S":

1. Pergunte a moeda de cobranca
2. Pergunte o cambio manual: quantas unidades da moeda local valem 1 unidade da moeda de cobranca
3. Grave `billing_currency` e `exchange_rate_to_local`

Se `billing_currency == currency`, force ambos para null.

## Resumo e confirmacao

Mostre uma tabela em pt-br com as colunas `Campo | Valor | Origem`, onde Origem e `digitado`, `pesquisado (<fontes>, <data>)` ou `padrao`. Linhas:

- Modo (sei informar ou pesquisar)
- Pais
- Moeda
- Regiao (cidade e estado, ou "nao informada")
- Senioridade canonica e label amigavel
- Taxa hora e modo
- Markup de projeto
- Regime tributario, fator aproximado, tipo do fator e fonte
- Aviso se o fator inclui VAT, IVA, ISS ou imposto destacado
- Modelos de cobranca
- Perfil de cliente
- Cobranca estrangeira

Campos sem origem registrada (pais, moeda, regiao, senioridade, perfil de cliente, cobranca estrangeira) mostram `digitado` na coluna Origem quando o usuario respondeu e `padrao` quando o agente preencheu sem perguntar.

Pergunte literalmente: "Deseja salvar este perfil? S/N"

## Persistencia

Construa o JSON conforme `profile-schema.json`:

```
schema_version = "1.2"
created_at = <timestamp ISO 8601 UTC>
mode
country
currency
region
seniority
hourly_rate
hourly_rate_mode
monthly_target_income
billable_hours_per_month
margin_percent
tax_regime
tax_factor
tax_factor_kind
tax_factor_source
includes_vat
vat_pass_through_warning
tax_regime_confidence
pricing_models
client_profile
billing_currency
exchange_rate_to_local
research_ref
field_origins = {
  hourly_rate:    { origin, source, researched_at }
  margin_percent: { origin, source, researched_at }
  tax_regime:     { origin, source, researched_at }
  pricing_models: { origin, source, researched_at }
}
```

Regras:

- `hourly_rate_mode` aceita `direto`, `derivado` ou `pesquisado`
- Quando `hourly_rate_mode = "pesquisado"`, `monthly_target_income` e `billable_hours_per_month` ficam `null`
- Quando `hourly_rate_mode = "derivado"`, os dois campos sao obrigatorios
- `research_ref` e o caminho de `_pricing/research/hourly_rate.json` quando a taxa hora foi pesquisada, senao `null`
- `region` e `null` em modo experiente ou quando o usuario pulou
- `origin` em cada entrada de `field_origins` e `digitado`, `pesquisado` ou `padrao`; `researched_at` so e preenchido quando `origin = "pesquisado"`

Valide mentalmente contra o schema. Se algo estiver ausente, refaca apenas a pergunta correspondente.

Grave `_reversa_sdd/_pricing/profile.json` e `_reversa_sdd/_pricing/profile.md` atomicamente. O `profile.md` repete a tabela do resumo, com a coluna Origem.

## Disclaimer do profile.md

Inclua:

```
Disclaimer: o fator de imposto registrado e uma reserva aproximada para orcamento, nao uma aliquota legal exata. Validacao tributaria real e responsabilidade do contador do usuario. Valores pesquisados ou padrao sao referencias, nao recomendacoes: revise antes de usar em proposta. Este arquivo contem dados financeiros sensiveis. Recomenda-se adicionar `_reversa_sdd/_pricing/profile.json` e `_reversa_sdd/_pricing/profile.md` ao `.gitignore` antes de commitar.
```

## Encerramento sem mudancas

Se o usuario cancelar antes de salvar:

1. Nao grave nada
2. Se um backup foi criado, restaure o `.bak` de volta para `profile.json`
3. Confirme: "Profile mantido sem alteracoes."

## Relatorio final

Imprima:

1. Caminho absoluto de `profile.json`, se gravado
2. Caminho absoluto de `profile.md`, se gravado
3. Caminho do backup, se houve sobrescrita
4. Caminho de `research/hourly_rate.json`, se a taxa hora foi pesquisada
5. Proximo passo:
   - se `advisor.json` existe com `tracks.product = true` e `_reversa_sdd/_pricing/product/product.json` nao existe, sugerir `/reversa-pricing-product`
   - senao, se existe feature ativa com tasks (`.reversa/active-requirements.json` com `feature-dir` valido contendo `tasks.md`, `actions.md` ou `to-do.md`) e `_reversa_sdd/_pricing/<feature>/size.json` ja existe sem `estimate.json`, sugerir `/reversa-pricing-estimate`
   - senao, se existe feature ativa com tasks, sugerir `/reversa-pricing-size`
   - senao, sugerir `/reversa-forward` para planejar a feature antes do size, explicando que o size le o planejamento da feature, nao o codigo
6. A frase: "Use `/reversa-pricing` a qualquer momento para ver o estado do time."

Termine com:

> Digite **CONTINUAR** para prosseguir conforme a sugestao acima.
