# Formula do ponto de equilibrio (breakeven-formula.md)

**Versao da formula:** 1.0

Documenta o calculo deterministico que o agente `reversa-pricing-product` aplica para cada modelo de venda de produto (assinatura, aluguel, licenca perpetua e loja de apps). A formula responde a uma unica pergunta: **quantos clientes pagantes cada preco da escada exige** para cobrir o custo e para atingir a meta de renda. Ela nao preve quantos clientes vao aparecer.

## Fonte e criterio

Ponto de equilibrio (break-even) e a quantidade vendida em que a receita liquida iguala o custo. E uma conta de contabilidade gerencial basica, nao um modelo de demanda. O Reversa usa a versao mais simples: custo fixo dividido pelo liquido por unidade, com arredondamento para cima porque nao existe meio cliente.

Referencia:

- Charles T. Horngren et al., *Cost Accounting: A Managerial Emphasis*, capitulo sobre analise custo-volume-lucro (CVP), Pearson
- Corporate Finance Institute, Break-Even Analysis: https://corporatefinanceinstitute.com/resources/accounting/break-even-analysis/

## Entradas

Vindas do profile:

```
tax_factor = profile.tax_factor          (reserva aproximada de imposto, 0 a 1)
currency   = profile.currency
```

Vindas da entrevista do product:

```
monthly_recurring_cost        custo mensal para manter rodando (>= 0)
monthly_target_income         renda mensal desejada (> 0)
development_cost_to_recover   investimento a recuperar (>= 0, 0 para ignorar)
recovery_horizon_months       meses para recuperar e avaliar venda unica (inteiro, 1 a 60)
price_ladders                 uma escada por unidade presente (mensal, anual, licenca), cada uma com 3 a 5 precos positivos, ordenados
```

Cada modelo usa a escada da sua unidade (`ladder_unit`): `mensal` para assinatura, aluguel mensal e loja com assinatura; `anual` para aluguel anual; `licenca` para licenca perpetua e loja com venda unica.

Vindas do modelo:

```
period_months        1 (mensal) ou 12 (anual) nos modelos recorrentes; null em venda unica
commission_percent   percentual da loja (0 no canal direto)
```

## Passo 1: liquido por preco

```
commission = commission_percent / 100          (0 no canal direto)
liquido(preco) = preco * (1 - commission) * (1 - tax_factor)
```

A comissao e descontada antes do imposto porque a loja retem a parte dela antes de repassar ao desenvolvedor. O `tax_factor` entra como reserva aproximada sobre o que chega ao desenvolvedor.

Guarda: se `liquido <= 0`, a linha fica `available = false` com `reason = "nao disponivel: o preco nao cobre comissao e imposto"` e nenhuma contagem e calculada.

## Passo 2: modelos recorrentes (assinatura, aluguel, loja com assinatura)

```
preco_mensal_equivalente = preco / period_months
liquido_mensal = liquido(preco_mensal_equivalente)
custo_mensal_total = monthly_recurring_cost + development_cost_to_recover / recovery_horizon_months
pagantes_para_cobrir_custo = ceil(custo_mensal_total / liquido_mensal)
pagantes_para_meta = ceil((custo_mensal_total + monthly_target_income) / liquido_mensal)
```

O investimento a recuperar e diluido em parcelas mensais iguais ao longo do horizonte. `custo_mensal_total` e calculado sem arredondar (precisao decimal completa). Quando `custo_mensal_total = 0`, `pagantes_para_cobrir_custo = 0`.

No aluguel anual (`period_months = 12`), o preco da escada e por ano e a tabela mostra tambem o equivalente mensal. Em assinatura e loja com assinatura, `period_months = 1` e `preco_mensal_equivalente = preco`.

## Passo 3: venda unica (licenca perpetua, loja com venda unica)

```
liquido_unidade = liquido(preco)
custo_horizonte = monthly_recurring_cost * recovery_horizon_months + development_cost_to_recover
meta_horizonte = monthly_target_income * recovery_horizon_months
unidades_para_cobrir_custo = ceil(custo_horizonte / liquido_unidade)
unidades_para_meta = ceil((custo_horizonte + meta_horizonte) / liquido_unidade)
unidades_por_mes_para_meta = ceil(unidades_para_meta / recovery_horizon_months)
```

Venda unica nao tem receita recorrente, entao a conta e feita sobre o horizonte inteiro: tudo que precisa ser coberto nos `recovery_horizon_months` meses, dividido pelo liquido de uma licenca. `custo_horizonte` e calculado sem arredondar (precisao decimal completa). Quando `custo_horizonte = 0`, `unidades_para_cobrir_custo = 0`.

## Passo 4: arredondamento

1. Arredondamento comercial, metade para cima (half up), calculado em decimal, nunca em float binario
2. Moeda com 2 casas decimais: `preco_mensal_equivalente` e `liquido` sao arredondados para 2 casas antes de qualquer divisao
3. Ordem no aluguel anual: `preco_mensal_equivalente = preco / 12` e arredondado a 2 casas primeiro; depois `liquido_mensal = liquido(preco_mensal_equivalente ja arredondado)`; depois `liquido_mensal` e arredondado a 2 casas; so entao o `ceil`
4. `custo_mensal_total` e `custo_horizonte` nao sao arredondados: entram na divisao com precisao decimal completa. O product.md imprime os dois com 2 casas na tabela de insumos, para que o leitor refaca a conta
5. As contagens usam o `liquido` ja arredondado, para que quem le a tabela consiga refazer a conta com os numeros impressos
6. Contagens sao inteiras, sempre com `ceil` (arredondar para cima): nao existe fracao de cliente
7. Percentuais de comissao sao usados como estao na tabela, sem arredondar

## Exemplo (a): app de assinatura na Google Play

```
profile:
  currency = BRL, tax_factor = 0.15

modelo:
  loja_de_apps, store = google_play, store_billing = assinatura
  recurrence = recurring, period_months = 1
  commission_percent = 15

entrevista:
  monthly_recurring_cost = 300.00
  monthly_target_income = 5000.00
  development_cost_to_recover = 0
  recovery_horizon_months = 12
  price_ladders = { mensal: [9.90, 19.90, 29.90] }    (ladder_unit do modelo = mensal)
```

Fatores:

```
commission = 15 / 100 = 0.15
(1 - commission) = 0.85
(1 - tax_factor) = 0.85
fator liquido = 0.85 * 0.85 = 0.7225
custo_mensal_total = 300.00 + 0 / 12 = 300.00
custo_mensal_total + monthly_target_income = 300.00 + 5000.00 = 5300.00
```

Preco 9,90:

```
preco_mensal_equivalente = 9.90 / 1 = 9.90
9.90 * 0.85 = 8.415
8.415 * 0.85 = 7.15275  ->  liquido_mensal = 7.15
pagantes_para_cobrir_custo = ceil(300.00 / 7.15) = ceil(41.958...) = 42
pagantes_para_meta = ceil(5300.00 / 7.15) = ceil(741.258...) = 742
```

Preco 19,90:

```
19.90 * 0.85 = 16.915
16.915 * 0.85 = 14.37775  ->  liquido_mensal = 14.38
pagantes_para_cobrir_custo = ceil(300.00 / 14.38) = ceil(20.862...) = 21
pagantes_para_meta = ceil(5300.00 / 14.38) = ceil(368.567...) = 369
```

Preco 29,90:

```
29.90 * 0.85 = 25.415
25.415 * 0.85 = 21.60275  ->  liquido_mensal = 21.60
pagantes_para_cobrir_custo = ceil(300.00 / 21.60) = ceil(13.888...) = 14
pagantes_para_meta = ceil(5300.00 / 21.60) = ceil(245.370...) = 246
```

Tabela resultante:

| Preco | Liquido por pagante/mes | Pagantes para cobrir custo | Pagantes para a meta |
|---:|---:|---:|---:|
| 9,90 BRL | 7,15 BRL | 42 | 742 |
| 19,90 BRL | 14,38 BRL | 21 | 369 |
| 29,90 BRL | 21,60 BRL | 14 | 246 |

Leitura: a 19,90 por mes voce precisa de 21 assinantes para cobrir o custo e 369 para a meta.

## Exemplo (b): sistema web vendido direto como licenca perpetua

```
profile:
  currency = BRL, tax_factor = 0.15

modelo:
  licenca_perpetua, canal direto
  recurrence = one_time, period_months = null
  commission_percent = 0

entrevista:
  monthly_recurring_cost = 0
  monthly_target_income = 4000.00
  development_cost_to_recover = 12000.00
  recovery_horizon_months = 12
  price_ladders = { licenca: [1500, 3000, 6000] }    (ladder_unit do modelo = licenca)
```

Fatores:

```
commission = 0 / 100 = 0
(1 - commission) = 1.00
(1 - tax_factor) = 0.85
fator liquido = 1.00 * 0.85 = 0.85
custo_horizonte = 0 * 12 + 12000.00 = 12000.00
meta_horizonte = 4000.00 * 12 = 48000.00
custo_horizonte + meta_horizonte = 12000.00 + 48000.00 = 60000.00
```

Preco 1500:

```
liquido_unidade = 1500 * 0.85 = 1275.00
unidades_para_cobrir_custo = ceil(12000.00 / 1275.00) = ceil(9.411...) = 10
unidades_para_meta = ceil(60000.00 / 1275.00) = ceil(47.058...) = 48
unidades_por_mes_para_meta = ceil(48 / 12) = ceil(4.0) = 4
```

Preco 3000:

```
liquido_unidade = 3000 * 0.85 = 2550.00
unidades_para_cobrir_custo = ceil(12000.00 / 2550.00) = ceil(4.705...) = 5
unidades_para_meta = ceil(60000.00 / 2550.00) = ceil(23.529...) = 24
unidades_por_mes_para_meta = ceil(24 / 12) = ceil(2.0) = 2
```

Preco 6000:

```
liquido_unidade = 6000 * 0.85 = 5100.00
unidades_para_cobrir_custo = ceil(12000.00 / 5100.00) = ceil(2.352...) = 3
unidades_para_meta = ceil(60000.00 / 5100.00) = ceil(11.764...) = 12
unidades_por_mes_para_meta = ceil(12 / 12) = ceil(1.0) = 1
```

Tabela resultante:

| Preco | Liquido por licenca | Licencas para cobrir custo (12 meses) | Licencas para a meta | Licencas por mes |
|---:|---:|---:|---:|---:|
| 1500,00 BRL | 1275,00 BRL | 10 | 48 | 4 |
| 3000,00 BRL | 2550,00 BRL | 5 | 24 | 2 |
| 6000,00 BRL | 5100,00 BRL | 3 | 12 | 1 |

Leitura: a 3000 por licenca voce precisa de 5 licencas em 12 meses para cobrir o investimento e 24 (2 por mes) para a meta.

## Limites

1. Nao modela churn (cancelamentos): o numero de pagantes e o que precisa estar ativo em cada mes, nao quantos precisam entrar
2. Nao modela conversao (quantos visitantes ou testes viram pagantes)
3. Nao modela taxa de pagamento (gateway, cartao, boleto, Pix)
4. Nao modela descontos, cupons nem periodo de trial
5. Nao modela imposto especifico de venda de produto ou licenciamento: usa o `tax_factor` do profile como reserva aproximada, que pode nao ser o regime certo para esse tipo de venda
6. Nao modela cambio: precos e custos ficam na moeda do profile
7. O investimento a recuperar e diluido em parcelas iguais; a formula nao calcula juros nem valor do dinheiro no tempo
8. A formula diz o que cada preco exige; a escolha do preco e do usuario
