# Politica de pesquisa (research-policy.md)

**Versao da politica:** 1.0
**Data de referencia:** 2026-05

Documenta as regras que o agente `reversa-pricing-research` segue para buscar referencias de preco na web. E o unico agente do time de precificacao com acesso a internet; todos os outros leem o resultado gravado em `_pricing/research/<kind>.json`.

## Disclaimer obrigatorio

O resultado da pesquisa e referencia de mercado, nao recomendacao de preco. Fontes publicas trazem com frequencia salario mensal ou anual, nao taxa freelance direta; quando a linha deriva uma taxa hora a partir de salario, o campo `derived` deve ser `true` e a `note` deve mostrar a conta. Nenhum markup, imposto ou comissao e somado aqui. O valor so entra no profile ou no product depois que o usuario confirma no agente chamador.

## 1. Ferramenta de busca por engine

| Engine | Ferramenta de busca | Ferramenta de leitura de pagina | Como detectar |
|---|---|---|---|
| Claude Code | `WebSearch` | `WebFetch` | aparece na lista de ferramentas da sessao; pode estar adiada (deferred) e precisar ser carregada antes do uso |
| Codex | busca web nativa do Codex | leitura de URL do Codex | a sessao expoe uma ferramenta de busca na web; se o ambiente estiver sem rede, ela nao aparece ou falha |
| Gemini CLI | `google_web_search` | `web_fetch` | aparece na lista de ferramentas da sessao |
| Cursor | `@web` | abrir URL pelo proprio `@web` | o contexto aceita a mencao `@web` |
| Outro engine | qualquer ferramenta cuja funcao seja buscar na web | qualquer ferramenta que abra URL | verificar a lista de ferramentas; nome diferente nao importa, a funcao importa |

Regras:

1. Leitura de pagina sem busca nao conta como busca. Serve apenas para abrir a URL encontrada e confirmar o numero
2. Sem ferramenta de busca na sessao: gravar `available = false`, `reason = "engine_sem_busca"`, avisar "Este engine nao tem busca na web. Informe o valor manualmente no agente chamador." e encerrar
3. Ferramenta presente mas todas as buscas com erro: gravar `available = false`, `reason = "busca_falhou"`, manter `queries_run` com as consultas tentadas
4. Nunca inventar fonte, URL, numero ou data. Numero de memoria nao substitui busca

## 2. Limite de buscas

| Regra | Valor |
|---|---|
| Buscas por execucao | ate 6 |
| Buscas por nivel geografico (`hourly_rate`) | ate 2 |
| Registro | toda consulta executada vai em `queries_run`, com ou sem resultado util |

Leitura de pagina (abrir URL encontrada) nao conta no limite de 6, mas abra apenas paginas que apareceram em uma busca registrada.

## 3. Cadeia geografica (so `hourly_rate`)

| Ordem | Nivel | Quando tentar | Aceitar quando |
|---|---|---|---|
| 1 | `cidade` | `region.city` informado | pelo menos 2 fontes aceitas com numero |
| 2 | `estado` | `region.state` informado | pelo menos 2 fontes aceitas com numero |
| 3 | `pais` | sempre | pelo menos 2 fontes aceitas com numero |

Regras:

1. Em cada nivel, ate 2 buscas. Ao atingir 2 fontes aceitas, parar e gravar `level_used` com esse nivel
2. Nivel que nao atingiu 2 fontes entra em `levels_tried` e as fontes aceitas que ele produziu ficam guardadas, com `note` indicando o nivel ("nivel cidade")
3. Se nenhum nivel atingiu 2 fontes, `level_used` e o nivel mais especifico que produziu pelo menos 1 fonte aceita, e `weak = true`
4. Se nenhum nivel produziu fonte aceita, `available = false`, `reason = "sem_fontes_aceitas"`
5. Avisar o usuario antes de comecar que dado por cidade e escasso e que o pais costuma ser o nivel que funciona
6. `comparables` e `store_commission` nao usam a cadeia: `level_used = null`, `levels_tried = []`

## 4. Fontes aceitas e rejeitadas

### Aceitas

| Categoria | Exemplos | Serve para | Unidade tipica |
|---|---|---|---|
| Portal de salario | Glassdoor, salario.com.br, Indeed, Payscale | `hourly_rate` | `mes` ou ano, normalizar para `hora` |
| Plataforma freelance | Upwork, Workana, 99freelas | `hourly_rate` | `hora` |
| Estatistica oficial | CAGED (BR), BLS (US), INE (PT, ES) e equivalentes do pais | `hourly_rate` | `mes`, ano ou `hora` |
| Documentacao oficial da loja | Google Play Console Help, Apple Developer, Microsoft Learn, Steamworks Documentation | `store_commission` | `percentual` |
| Pagina de preco de produto | pagina "precos" ou "planos" do proprio produto, loja do produto | `comparables` | `mes` ou `licenca` |

Toda fonte aceita precisa de nome, URL, numero, unidade, data de acesso e, quando houver, data de publicacao ou atualizacao em `note`.

### Rejeitadas

| Situacao | `reason` em `skipped_sources` | Regra |
|---|---|---|
| Forum, comentario ou pergunta sem numero | `sem_numero` | registrar e seguir |
| Texto gerado por IA sem fonte identificavel | `texto_gerado_sem_fonte` | registrar e seguir |
| Pagina sem data identificavel (sem data de publicacao, atualizacao nem referencia de ano) | `sem_data` | registrar e seguir |
| Fonte em moeda diferente de `currency` | `moeda_diferente` | registrar, nunca converter |
| Unidade incompativel com o modelo pedido em `comparables` | `unidade_diferente` | manter a unidade da maioria das fontes |
| Fora do escopo (outra senioridade, outro pais, outra categoria de produto, outra loja) | `fora_do_escopo` | registrar e seguir |
| Noticia, blog ou forum com percentual de comissao | `fora_do_escopo` | em `store_commission` so pagina oficial conta |

## 5. Normalizacao

| Entrada da fonte | Conta | `derived` | Onde vale |
|---|---|---|---|
| taxa hora direta | nenhuma | `false` | `hourly_rate` |
| salario mensal | dividir por 160 (160 h/mes) | `true` | `hourly_rate` |
| salario anual | dividir por 1920 (1920 h/ano) | `true` | `hourly_rate` |
| plano anual | dividir por 12 | `true` | `comparables` |
| preco mensal, por licenca ou percentual | nenhuma | `false` | `comparables`, `store_commission` |

Regras:

1. Arredondar para 2 casas
2. Escrever a conta em `note` ("salario mensal 8000 / 160")
3. Nunca somar markup, imposto, encargos ou comissao. Isso e papel do profile, do estimate e do product
4. Nunca converter moeda. Fonte em outra moeda e rejeitada
5. Fonte com um unico numero grava `value_min = value_max`
6. Fonte cujo valor seja 0 (plano gratuito, "a combinar") nao e aceita: vai para `skipped_sources` com `reason = "sem_numero"`. Assim o ratio da confianca nunca divide por zero

## 6. Confianca

```
ponto_medio(fonte) = (value_min + value_max) / 2
ratio              = maior ponto_medio / menor ponto_medio   (entre as fontes aceitas)
```

| Condicao | `confidence` | `weak` |
|---|---|---|
| 2 ou mais fontes e ratio menor que 2 | `high` | `false` |
| 2 ou mais fontes e ratio de 2 a 3 | `medium` | `false` |
| 2 ou mais fontes e ratio maior que 3 | `low` | `false` |
| 1 fonte aceita | `low` | `true` |
| 0 fontes aceitas | `null` (com `available = false`) | `true` |

Faixa:

```
faixa_min     = menor value_min entre as fontes aceitas
faixa_max     = maior value_max entre as fontes aceitas
faixa_mediana = mediana dos pontos medios
```

## 7. Data de acesso

`accessed_at` e a data de hoje conhecida pela sessao, no formato `AAAA-MM-DD`, igual em todas as fontes da execucao. Se a sessao nao souber a data, perguntar ao usuario: "Qual a data de hoje? Formato AAAA-MM-DD." A data de publicacao da fonte, quando houver, vai em `note`, nunca em `accessed_at`.

## 8. Apresentacao

Tabela das fontes aceitas:

```
| Fonte | Valor | Unidade | Derivado | URL |
```

Depois a linha:

```
Faixa: <faixa_min> a <faixa_max> <currency>, mediana <faixa_mediana>, nivel <level_used>, confianca <confidence>
```

Em `store_commission`, `<currency>` vira `%`. `level_used` nulo vira `nao se aplica`. Com `weak = true`, acrescentar `(fraca: menos de 2 fontes)`. Fechar lembrando que e referencia, nao recomendacao.

## 9. Gravacao

| Regra | Valor |
|---|---|
| Pasta unica de escrita | `<output_folder>/_pricing/research/` |
| Arquivos | `<kind>.json` e `<kind>.md` |
| Recalculo | perguntar "Ja existe research/<kind>.json. Deseja recalcular? S/N"; "N" encerra sem mudancas; "S" renomeia para `<arquivo>.bak.<YYYYMMDD-HHMMSS>` antes de gravar |
| Escrita | atomica (tempfile mais rename), UTF-8 sem BOM |
| Proibido | escrever em `profile.json`, `product.json`, `advisor.json`, codigo legado, Discovery, Forward ou `.reversa/` |

## 10. Exemplos de consultas por kind

Substituir os campos entre `<>` pelos inputs. Usar pt-br para paises de lingua portuguesa e ingles para os demais ou quando a fonte for internacional (Upwork, Payscale, BLS, documentacao de loja). Rotulo leigo de senioridade em pt-br: junior -> "junior", mid -> "pleno", senior -> "senior", staff_lead -> "lider tecnico", principal -> "principal". Em ingles: junior -> "junior", mid -> "mid-level", senior -> "senior", staff_lead -> "staff" ou "lead", principal -> "principal".

### `hourly_rate`

pt-br:

1. `valor hora desenvolvedor <senioridade leiga> <cidade> <estado> <ano>`
2. `salario desenvolvedor <senioridade leiga> <estado> glassdoor OR salario.com.br`
3. `freelancer desenvolvedor <senioridade leiga> valor hora <pais> workana OR 99freelas`

ingles:

1. `software developer hourly rate <city> <country> <seniority> <ano>`
2. `<seniority> software developer salary <state> glassdoor OR payscale OR indeed`
3. `upwork software developer hourly rate <country> <seniority>`

Ordem sugerida na cadeia: consulta 1 no nivel cidade, consulta 2 no nivel estado, consulta 3 no nivel pais, completando cada nivel com a segunda busca quando a primeira trouxer menos de 2 fontes.

### `comparables`

pt-br:

1. `<descricao> precos planos assinatura mensal`
2. `software <descricao> quanto custa licenca`
3. `alternativas a <descricao> preco <pais>`

ingles:

1. `<description> pricing plans per month`
2. `<description> software one-time license price`
3. `<description> alternatives pricing <country>`

Para `loja_de_apps`, acrescentar o nome da loja a consulta (`google play`, `app store`, `microsoft store`, `steam`) e preferir a pagina do produto dentro da loja.

### `store_commission`

pt-br:

1. `comissao google play desenvolvedor percentual taxa de servico`
2. `taxa app store small business program percentual`
3. `comissao microsoft store apps percentual`

ingles:

1. `google play service fee percentage developer`
2. `app store commission small business program`
3. `steam revenue share distribution agreement`

Para `outra`, usar `<nome da loja> developer commission percentage` e `<nome da loja> comissao desenvolvedor`. So a pagina oficial da loja conta.

## 11. Como estender

Para adicionar engine, fonte ou kind:

1. Engine novo: acrescentar linha na tabela da secao 1 com ferramenta de busca, ferramenta de leitura e como detectar
2. Fonte nova: acrescentar na tabela de aceitas com categoria, kind que atende e unidade tipica; fonte sem numero ou sem data continua rejeitada
3. Kind novo: definir inputs, unidade de saida, se usa cadeia geografica, exemplos de consulta (3 em pt-br e 3 em ingles) e acrescentar o valor ao enum `kind` em `research-schema.json`
4. Mudanca de regra de confianca, normalizacao ou limite de buscas: bump na versao da politica e no `schema_version` quando o JSON mudar
