# Template do advisor.md

Este e o template Markdown que o agente `reversa-pricing-advisor` usa para gerar `_reversa_sdd/_pricing/advisor.md`. Substitua todos os `<placeholders>` pelos valores reais. Mantenha a estrutura fixa.

```markdown
# Modelo de Venda

**Gerado em:** <created_at_local_legivel>
**Versao do schema:** 1.0
**Regras:** `sale-models.md` v1.0
**Artefato JSON:** `<output_folder>/_pricing/advisor.json`

## Suas respostas

| Pergunta | Resposta |
|---|---|
| Tipo de software | <software_type_legivel> (`<software_type>`) |
| Origem | <origin_legivel> (`<origin>`) |
| Quem paga | <payer_legivel> (`<payer>`) |
| Quem hospeda | <hosting_legivel> (`<hosting>`) |
| Custo mensal para manter rodando | <monthly_recurring_cost ou "nao sei"> |
| Preferencia de renda | <income_preference_legivel> (`<income_preference>`) |

## Modelos de venda avaliados

| # | Modelo | Viavel | Pontos | Por que | Riscos |
|---|---|---|---|---|---|
| 1 | <nome_leigo> (`<model>`) | <Sim ou Nao> | <score> | <reason_pt_br> | <risks_pt_br separados por ponto e virgula> |
| 2 | ... | ... | ... | ... | ... |
| 3 | ... | ... | ... | ... | ... |
| 4 | ... | ... | ... | ... | ... |
| 5 | ... | ... | ... | ... | ... |

<nota_hosting_nao_sei_se_aplicavel>

## Modelos escolhidos

- `<model>`: <nome_leigo>
- ...

<bloco_forced_models_se_aplicavel>

<bloco_loja_se_aplicavel>

Trilhas: servico = <true ou false>, produto = <true ou false>

## Proximo passo

<proximo_passo_pt_br>

## Disclaimer

Este e um guia de opcoes, nao consultoria comercial. A viabilidade e calculada por regras simples sobre as suas respostas. Percentuais de comissao de loja vem de `store-commissions.md` e mudam com o tempo; confira a pagina oficial antes de decidir. Recomenda-se adicionar `_reversa_sdd/_pricing/advisor.json` e `_reversa_sdd/_pricing/advisor.md` ao `.gitignore` antes de commitar.
```

## Rotulos legiveis

| Campo | Valor | Rotulo |
|---|---|---|
| software_type | app_mobile | App mobile |
| software_type | app_desktop | App desktop |
| software_type | sistema_web | Sistema web |
| software_type | api_servico | API ou servico |
| software_type | ferramenta_interna | Ferramenta interna |
| software_type | outro | Outro |
| origin | encomenda | Sob encomenda para um cliente |
| origin | muitos | Para vender a muitos |
| origin | ambos | Os dois |
| payer | pessoa_fisica | Pessoa fisica |
| payer | empresa | Empresa |
| payer | ambos | Os dois |
| hosting | eu_hospedo | Eu hospedo e o cliente acessa |
| hosting | cliente_instala | O cliente instala na maquina ou servidor dele |
| hosting | nao_sei | Nao sei |
| income_preference | unico | Pagamento unico |
| income_preference | recorrente | Renda recorrente |
| income_preference | tanto_faz | Tanto faz |

## Blocos condicionais

Nota de hosting desconhecido, quando `answers.hosting = nao_sei`:

```markdown
> Descubra quem vai hospedar: isso decide entre assinatura e aluguel/licenca.
```

Bloco de modelos forcados, quando `forced_models` nao esta vazio:

```markdown
> Mantidos mesmo sendo inviaveis pelas regras: `<model>` (<reason_pt_br>), ...
```

Bloco de loja, quando `loja_de_apps` esta em `selected_models`:

```markdown
Loja: <store_legivel> (`<store>`), cobranca dentro da loja: <unica ou assinatura>. Comissao de referencia: <commission_percent>% (fonte: <source>, data de referencia <data>), conforme `store-commissions.md`. Se a tabela nao estiver disponivel, escreva "comissao: ver store-commissions.md no product".
```

## Proximo passo (`<proximo_passo_pt_br>`)

Uma frase, conforme o roteamento do SKILL.md:

- profile.json ausente: "Rode `/reversa-pricing-profile` para registrar como voce cobra."
- profile presente, `tracks.product = true` e product.json ausente: "Rode `/reversa-pricing-product` para ver quantos clientes cada preco exige."
- profile presente, `tracks.service = true` (sem produto pendente), feature ativa com tasks e sem size.json: "Rode `/reversa-pricing-size` para medir a feature ativa."
- qualquer outro caso: "Use `/reversa-pricing` para ver o estado do time e o proximo passo."
