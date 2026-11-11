# Template do product.md

Este e o template Markdown que o agente `reversa-pricing-product` usa para gerar `_reversa_sdd/_pricing/product/product.md`. Substitua todos os `<placeholders>` pelos valores reais. Mantenha a estrutura fixa. Repita o bloco "Modelo" uma vez por modelo calculado, na ordem de `advisor.selected_models`.

```markdown
# Ponto de Equilibrio do Produto

**Gerado em:** <created_at_local_legivel>
**Versao da formula:** <formula_version>
**Moeda:** <currency>

**Pre-requisitos consumidos:**
- Advisor: `<output_folder>/_pricing/advisor.json` (modelos escolhidos: <selected_models_legivel>)
- Profile: `<output_folder>/_pricing/profile.json` (reserva de imposto `<tax_factor>`, regime `<tax_regime>`)
- Research: `<research_ref_ou_"nao usado">`

## Entradas

| Item | Valor |
|---|---|
| Custo mensal para manter rodando | <monthly_recurring_cost> <currency> |
| Renda mensal desejada | <monthly_target_income> <currency> |
| Investimento a recuperar | <development_cost_to_recover> <currency> |
| Horizonte de recuperacao | <recovery_horizon_months> meses |
| Reserva de imposto (profile) | <tax_factor_percent>% |
| Custo mensal total (custo mais investimento diluido) | <custo_mensal_total_2_casas> <currency> |
| Custo no horizonte (custo vezes horizonte mais investimento) | <custo_horizonte_2_casas> <currency> |
| Escada de precos por mes | <price_ladders.mensal.precos_legivel> (<price_ladders.mensal.origem>) |
| Escada de precos por ano | <price_ladders.anual.precos_legivel> (<price_ladders.anual.origem>) |
| Escada de precos por licenca | <price_ladders.licenca.precos_legivel> (<price_ladders.licenca.origem>) |

Inclua apenas as linhas de escada cujo grupo existe em `inputs.price_ladders`. `custo_mensal_total` e `custo_horizonte` sao gravados sem arredondar no product.json e impressos aqui com 2 casas, para que o leitor refaca a conta.

<aviso_tax_factor_zero_se_aplicavel>

## Modelo: <nome_leigo_do_modelo>

**Canal:** <direto ou loja: nome da loja>
**Cobranca:** <mensal, anual ou venda unica>
**Escada usada:** <por mes, por ano ou por licenca, conforme ladder_unit>
**Comissao:** <commission_percent>% (<commission_source>) ou "nao se aplica" no canal direto

<se recorrente>

| Preco | Liquido por pagante/mes | Pagantes para cobrir custo | Pagantes para a meta |
|---:|---:|---:|---:|
| <preco> <currency> | <liquido> <currency> | <pagantes_para_cobrir_custo> | <pagantes_para_meta> |

<se period_months = 12, acrescentar coluna "Equivalente mensal" depois de Preco>

<se venda unica>

| Preco | Liquido por licenca | Licencas para cobrir custo (<recovery_horizon_months> meses) | Licencas para a meta | Licencas por mes |
|---:|---:|---:|---:|---:|
| <preco> <currency> | <liquido> <currency> | <unidades_para_cobrir_custo> | <unidades_para_meta> | <unidades_por_mes_para_meta> |

</se>

Linhas indisponiveis aparecem como "nao disponivel: <reason>" em todas as colunas de contagem.

**Leitura:** <reading_pt_br>

<nota_de_excecoes_da_loja_se_aplicavel>

## Resumo

| Modelo | Menor exigencia (preco mais alto) | Maior exigencia (preco mais baixo) |
|---|---|---|
| <nome_leigo_do_modelo> | a <preco_mais_alto>: <N> para o custo, <M> para a meta | a <preco_mais_baixo>: <N> para o custo, <M> para a meta |

## Como escolher

<guidance_pt_br>

Escolha o ponto da escada que voce acredita conseguir vender; a tabela diz o que cada ponto exige. Nenhum preco desta lista e recomendacao: a calculadora nao sabe quantos clientes existem para o seu produto.

## Limites

Esta calculadora nao modela churn, conversao, taxa de pagamento (gateway), descontos, trial, imposto especifico de produto nem cambio. O investimento a recuperar e diluido em parcelas iguais ao longo do horizonte, sem juros.

## Disclaimer

Esta calculadora mostra o que cada preco exige, nao quantos clientes voce vai conseguir. Nao modela churn, conversao nem taxas de pagamento. A reserva de imposto vem do profile e pode nao ser o regime certo para venda de produto; valide com o contador. Comissoes de loja mudam; confira a pagina oficial. Recomenda-se adicionar `_reversa_sdd/_pricing/product/` ao `.gitignore`.
```

## Nomes leigos dos modelos

| model | nome_leigo_do_modelo |
|---|---|
| licenca_perpetua | Licenca perpetua (venda unica, o cliente instala) |
| aluguel | Aluguel (licenca por periodo, o cliente instala e renova) |
| assinatura | Assinatura (voce hospeda, cobranca recorrente) |
| loja_de_apps | Loja de apps: <nome da loja> (<venda unica ou assinatura dentro da loja>) |

## Aviso de tax_factor zero

Quando `profile.tax_factor = 0` (regime `outro` ou `not_computed`), inclua depois da tabela de entradas:

```markdown
> Reserva de imposto nao computada: o profile usa regime `outro`. Os liquidos abaixo nao descontam imposto; o calculo fica a cargo do contador.
```

## Nota de excecoes da loja

Quando `channel = loja`, inclua depois da leitura a linha de excecoes copiada de `store-commissions.md`:

```markdown
> Comissao de <nome da loja>: <commission_percent>% padrao. Excecoes conhecidas: <faixas e excecoes conhecidas>. Fonte: <source>, data de referencia <data>.
```

## Formato de numeros

No product.md e no chat, use separador decimal em pt-br (19,90) e o codigo da moeda depois do numero. No product.json, numeros ficam no formato JSON (19.90).
