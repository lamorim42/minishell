# Catalogo de modelos de venda e regras de viabilidade (versao 1.0)

Este arquivo e a referencia do agente `reversa-pricing-advisor`. Ele define os cinco modelos de venda, as regras deterministicas de pontuacao e bloqueio, os riscos padrao de cada modelo e a ordem de apresentacao. As regras sao simples de proposito: o objetivo e orientar quem nunca precificou software, nao prever mercado.

## Catalogo

Chaves canonicas, nesta ordem de catalogo. A ordem do catalogo desempata quando dois modelos tem a mesma pontuacao.

| # | key | Nome leigo | Trilha | Recorrencia |
|---|---|---|---|---|
| 1 | `servico_sob_medida` | Cobrar pelo trabalho de construir ou evoluir para um cliente | servico | nao se aplica |
| 2 | `licenca_perpetua` | Vender a licenca uma vez, o cliente instala e usa para sempre | produto | venda unica |
| 3 | `aluguel` | Licenca por periodo (mensal ou anual), o cliente instala e renova | produto | recorrente |
| 4 | `assinatura` | Acesso continuo ao software que voce hospeda, cobranca mensal ou anual | produto | recorrente |
| 5 | `loja_de_apps` | Publicar em loja (Google Play, App Store, Microsoft Store, Steam), venda unica ou assinatura dentro da loja, com comissao | produto | depende de `store_billing` |

Trilha `servico` leva ao pipeline por feature (`size` e `estimate`). Trilha `produto` leva ao `product` (ponto de equilibrio).

## Entradas

As regras usam as seis respostas da entrevista, com os valores canonicos:

```
software_type           app_mobile | app_desktop | sistema_web | api_servico | ferramenta_interna | outro
origin                  encomenda | muitos | ambos
payer                   pessoa_fisica | empresa | ambos
hosting                 eu_hospedo | cliente_instala | nao_sei
monthly_recurring_cost  number >= 0, ou null quando recurring_cost_known = false
income_preference       unico | recorrente | tanto_faz
```

## Regras de pontuacao e bloqueio

Para cada modelo, `score` comeca em 0 e soma os pontos das regras que batem. Um bloqueio nao zera o score: ele apenas torna o modelo inviavel. O score continua sendo calculado e exibido.

```
viable = score >= 1 AND sem bloqueio
```

Comparacoes com `monthly_recurring_cost`: quando o valor e `null` (usuario respondeu "nao sei"), nenhuma regra de custo pontua, nem a de `> 0` nem a de `= 0`.

### `servico_sob_medida`

| Regra | Pontos | Frase para o motivo |
|---|---|---|
| `origin` in {encomenda, ambos} | +2 | foi feito sob encomenda, entao ha quem pague pelo trabalho |
| `payer` in {empresa, ambos} | +1 | empresas costumam contratar desenvolvimento e evolucao |
| `software_type = ferramenta_interna` | +1 | ferramenta interna e trabalho feito para quem a usa |

Sem bloqueio. Se o score for 0, o motivo e: "nao ha cliente que encomende o trabalho".

### `assinatura`

| Regra | Pontos | Frase para o motivo |
|---|---|---|
| `hosting = eu_hospedo` | +2 | voce hospeda, entao pode cobrar pelo acesso continuo |
| `monthly_recurring_cost > 0` | +1 | ha custo mensal, e a cobranca recorrente cobre esse custo |
| `income_preference` in {recorrente, tanto_faz} | +1 | voce aceita renda recorrente |
| `software_type` in {sistema_web, api_servico} | +1 | sistemas web e APIs sao acessados pela rede, o formato natural da assinatura |

Bloqueio: `software_type = ferramenta_interna`, motivo "ferramenta interna nao se vende a terceiros".

### `aluguel`

| Regra | Pontos | Frase para o motivo |
|---|---|---|
| `hosting = cliente_instala` | +2 | o cliente instala, entao a licenca pode ter prazo e renovacao |
| `income_preference` in {recorrente, tanto_faz} | +1 | voce aceita renda recorrente |
| `monthly_recurring_cost > 0` | +1 | ha custo mensal, e a renovacao ajuda a cobrir |

Bloqueios:

- `hosting = eu_hospedo`, motivo "voce hospeda: o modelo equivalente e assinatura"
- `software_type = ferramenta_interna`, motivo "ferramenta interna nao se vende a terceiros"

### `licenca_perpetua`

| Regra | Pontos | Frase para o motivo |
|---|---|---|
| `hosting = cliente_instala` | +2 | o cliente instala e usa por conta propria |
| `income_preference` in {unico, tanto_faz} | +1 | voce aceita pagamento unico |
| `monthly_recurring_cost = 0` | +1 | sem custo mensal, a venda unica nao deixa conta aberta |

Risco obrigatorio quando `monthly_recurring_cost > 0` ou `null`: "venda unica nao cobre custo mensal continuo".

Bloqueios:

- `hosting = eu_hospedo`, motivo "voce hospeda: o modelo equivalente e assinatura"
- `software_type = ferramenta_interna`, motivo "ferramenta interna nao se vende a terceiros"

### `loja_de_apps`

| Regra | Pontos | Frase para o motivo |
|---|---|---|
| `software_type` in {app_mobile, app_desktop} | +2 | apps mobile e desktop sao distribuidos por lojas |
| `payer` in {pessoa_fisica, ambos} | +1 | pessoas fisicas compram em loja |
| `origin` in {muitos, ambos} | +1 | feito para muitos, e a loja alcanca muitos |

Bloqueio: `software_type` in {sistema_web, api_servico, ferramenta_interna}, motivo "lojas de apps distribuem apps mobile e desktop".

Risco obrigatorio, sempre: "a loja cobra comissao sobre cada venda; ver tabela em store-commissions.md".

### `hosting = nao_sei`

Nenhum modelo recebe os pontos de hosting. Acrescentar a nota, em `notes_pt_br` e abaixo da tabela: "descubra quem vai hospedar: isso decide entre assinatura e aluguel/licenca".

## Como montar o motivo (`reason_pt_br`)

1. Se o modelo tem bloqueio: o motivo e a frase do bloqueio. Se mais de um bloqueio bater, use o primeiro na ordem listada acima.
2. Senao, se o score for maior que 0: junte as frases das regras que bateram, separadas por ponto e virgula, na ordem da tabela do modelo.
3. Senao (score 0, sem bloqueio): `servico_sob_medida` usa "nao ha cliente que encomende o trabalho"; os demais usam "nenhuma das suas respostas aponta para este modelo".

## Riscos padrao por modelo (`risks_pt_br`)

Linguagem leiga, 2 a 3 por modelo. Os riscos obrigatorios condicionais entram na frente da lista quando a condicao bate.

### `servico_sob_medida`

1. A renda para quando o projeto acaba: voce precisa de novos clientes o tempo todo.
2. Mudanca de escopo sem aditivo vira trabalho de graca.
3. Depender de um cliente grande deixa voce vulneravel se ele for embora.

### `licenca_perpetua`

1. (obrigatorio se `monthly_recurring_cost > 0` ou `null`) venda unica nao cobre custo mensal continuo
2. Depois da venda, cada atualizacao e suporte sao custo seu sem receita nova.
3. Copia e uso alem do combinado sao dificeis de controlar quando o cliente instala.

### `aluguel`

1. O cliente pode nao renovar, e a receita cai sem aviso.
2. Voce precisa de um jeito de bloquear o uso quando a licenca vence, e isso exige trabalho tecnico.
3. Dar suporte a varias versoes instaladas em maquinas diferentes custa tempo.

### `assinatura`

1. Servidor, APIs e suporte sao conta sua todo mes, mesmo com poucos assinantes.
2. Cancelamentos corroem a receita: e preciso repor clientes sempre.
3. Qualquer queda do servico vira reclamacao imediata de todos os assinantes.

### `loja_de_apps`

1. (obrigatorio, sempre) a loja cobra comissao sobre cada venda; ver tabela em store-commissions.md
2. A loja pode rejeitar ou remover o app por regra propria, e a revisao atrasa lancamentos.
3. O preco fica exposto e comparado com concorrentes na mesma vitrine.

Nunca cite percentual de comissao de memoria. O percentual, a fonte e a data vem de `reversa-pricing-product/references/store-commissions.md` (pasta irma).

## Ordenacao

Score decrescente. Empate: ordem do catalogo (1 a 5). Os cinco modelos aparecem sempre, inclusive os inviaveis, para o usuario entender por que foram descartados.

## Tabela de apresentacao

```
# | Modelo | Viavel | Pontos | Por que | Riscos
```

`Modelo` mostra o nome leigo e a chave entre parenteses. `Viavel` mostra "Sim" ou "Nao". `Riscos` lista os riscos separados por ponto e virgula.

## Exemplos

### Exemplo A: app mobile para muitos, pessoa fisica, eu hospedo, custo 200, recorrente

Respostas:

```
software_type           = app_mobile
origin                  = muitos
payer                   = pessoa_fisica
hosting                 = eu_hospedo
monthly_recurring_cost  = 200   (recurring_cost_known = true)
income_preference       = recorrente
```

Contas:

```
servico_sob_medida   origin muitos: 0  | payer pessoa_fisica: 0 | nao e ferramenta_interna: 0      -> score 0, sem bloqueio, inviavel (score < 1)
assinatura           eu_hospedo: +2    | custo > 0: +1          | recorrente: +1 | app_mobile: 0    -> score 4, viavel
aluguel              cliente_instala: 0 | recorrente: +1        | custo > 0: +1                     -> score 2, BLOQUEIO (eu_hospedo), inviavel
licenca_perpetua     cliente_instala: 0 | unico/tanto_faz: 0    | custo = 0: 0                      -> score 0, BLOQUEIO (eu_hospedo), inviavel
loja_de_apps         app_mobile: +2    | pessoa_fisica: +1      | muitos: +1                        -> score 4, viavel
```

Ordenacao: assinatura (4) e loja_de_apps (4) empatam; assinatura vem antes por ser o item 4 do catalogo e loja_de_apps o item 5. Depois aluguel (2). Depois servico_sob_medida (0, item 1) e licenca_perpetua (0, item 2).

Tabela resultante:

| # | Modelo | Viavel | Pontos | Por que | Riscos |
|---|---|---|---|---|---|
| 1 | Acesso continuo ao software que voce hospeda, cobranca mensal ou anual (`assinatura`) | Sim | 4 | voce hospeda, entao pode cobrar pelo acesso continuo; ha custo mensal, e a cobranca recorrente cobre esse custo; voce aceita renda recorrente | Servidor, APIs e suporte sao conta sua todo mes, mesmo com poucos assinantes; Cancelamentos corroem a receita: e preciso repor clientes sempre; Qualquer queda do servico vira reclamacao imediata de todos os assinantes |
| 2 | Publicar em loja (Google Play, App Store, Microsoft Store, Steam), venda unica ou assinatura dentro da loja, com comissao (`loja_de_apps`) | Sim | 4 | apps mobile e desktop sao distribuidos por lojas; pessoas fisicas compram em loja; feito para muitos, e a loja alcanca muitos | a loja cobra comissao sobre cada venda; ver tabela em store-commissions.md; A loja pode rejeitar ou remover o app por regra propria, e a revisao atrasa lancamentos; O preco fica exposto e comparado com concorrentes na mesma vitrine |
| 3 | Licenca por periodo (mensal ou anual), o cliente instala e renova (`aluguel`) | Nao | 2 | voce hospeda: o modelo equivalente e assinatura | O cliente pode nao renovar, e a receita cai sem aviso; Voce precisa de um jeito de bloquear o uso quando a licenca vence, e isso exige trabalho tecnico; Dar suporte a varias versoes instaladas em maquinas diferentes custa tempo |
| 4 | Cobrar pelo trabalho de construir ou evoluir para um cliente (`servico_sob_medida`) | Nao | 0 | nao ha cliente que encomende o trabalho | A renda para quando o projeto acaba: voce precisa de novos clientes o tempo todo; Mudanca de escopo sem aditivo vira trabalho de graca; Depender de um cliente grande deixa voce vulneravel se ele for embora |
| 5 | Vender a licenca uma vez, o cliente instala e usa para sempre (`licenca_perpetua`) | Nao | 0 | voce hospeda: o modelo equivalente e assinatura | venda unica nao cobre custo mensal continuo; Depois da venda, cada atualizacao e suporte sao custo seu sem receita nova; Copia e uso alem do combinado sao dificeis de controlar quando o cliente instala |

Se o usuario escolher 1 e 2: `selected_models = ["assinatura", "loja_de_apps"]`, `forced_models = []`, `tracks = { service: false, product: true }`, e o agente faz as duas perguntas extras de loja.

### Exemplo B: sistema web sob encomenda, empresa, cliente instala, custo 0, unico

Respostas:

```
software_type           = sistema_web
origin                  = encomenda
payer                   = empresa
hosting                 = cliente_instala
monthly_recurring_cost  = 0   (recurring_cost_known = true)
income_preference       = unico
```

Contas:

```
servico_sob_medida   encomenda: +2      | empresa: +1            | nao e ferramenta_interna: 0       -> score 3, viavel
assinatura           eu_hospedo: 0      | custo > 0: 0           | unico: 0 | sistema_web: +1        -> score 1, sem bloqueio, viavel
aluguel              cliente_instala: +2 | unico: 0              | custo > 0: 0                      -> score 2, viavel
licenca_perpetua     cliente_instala: +2 | unico: +1             | custo = 0: +1                     -> score 4, viavel
loja_de_apps         sistema_web: 0     | empresa: 0             | encomenda: 0                      -> score 0, BLOQUEIO (sistema_web), inviavel
```

Ordenacao: licenca_perpetua (4), servico_sob_medida (3), aluguel (2), assinatura (1), loja_de_apps (0). Nao ha empate.

Tabela resultante:

| # | Modelo | Viavel | Pontos | Por que | Riscos |
|---|---|---|---|---|---|
| 1 | Vender a licenca uma vez, o cliente instala e usa para sempre (`licenca_perpetua`) | Sim | 4 | o cliente instala e usa por conta propria; voce aceita pagamento unico; sem custo mensal, a venda unica nao deixa conta aberta | Depois da venda, cada atualizacao e suporte sao custo seu sem receita nova; Copia e uso alem do combinado sao dificeis de controlar quando o cliente instala |
| 2 | Cobrar pelo trabalho de construir ou evoluir para um cliente (`servico_sob_medida`) | Sim | 3 | foi feito sob encomenda, entao ha quem pague pelo trabalho; empresas costumam contratar desenvolvimento e evolucao | A renda para quando o projeto acaba: voce precisa de novos clientes o tempo todo; Mudanca de escopo sem aditivo vira trabalho de graca; Depender de um cliente grande deixa voce vulneravel se ele for embora |
| 3 | Licenca por periodo (mensal ou anual), o cliente instala e renova (`aluguel`) | Sim | 2 | o cliente instala, entao a licenca pode ter prazo e renovacao | O cliente pode nao renovar, e a receita cai sem aviso; Voce precisa de um jeito de bloquear o uso quando a licenca vence, e isso exige trabalho tecnico; Dar suporte a varias versoes instaladas em maquinas diferentes custa tempo |
| 4 | Acesso continuo ao software que voce hospeda, cobranca mensal ou anual (`assinatura`) | Sim | 1 | sistemas web e APIs sao acessados pela rede, o formato natural da assinatura | Servidor, APIs e suporte sao conta sua todo mes, mesmo com poucos assinantes; Cancelamentos corroem a receita: e preciso repor clientes sempre; Qualquer queda do servico vira reclamacao imediata de todos os assinantes |
| 5 | Publicar em loja (Google Play, App Store, Microsoft Store, Steam), venda unica ou assinatura dentro da loja, com comissao (`loja_de_apps`) | Nao | 0 | lojas de apps distribuem apps mobile e desktop | a loja cobra comissao sobre cada venda; ver tabela em store-commissions.md; A loja pode rejeitar ou remover o app por regra propria, e a revisao atrasa lancamentos; O preco fica exposto e comparado com concorrentes na mesma vitrine |

Observe que `licenca_perpetua` nao recebe o risco "venda unica nao cobre custo mensal continuo" porque o custo mensal informado foi 0.

Se o usuario escolher 1 e 2: `selected_models = ["licenca_perpetua", "servico_sob_medida"]`, `forced_models = []`, `tracks = { service: true, product: true }`. Se escolher 5 e confirmar "S" em "Quer manter mesmo assim?", `loja_de_apps` entra em `selected_models` e em `forced_models`.
