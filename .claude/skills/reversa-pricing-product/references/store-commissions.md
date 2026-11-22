# Catalogo de comissoes de lojas de apps

**Versao da tabela:** 1.0
**Data de referencia:** 2026-05

Catalogo estatico usado pelo agente `reversa-pricing-product` para descontar a comissao da loja no calculo de ponto de equilibrio do modelo `loja_de_apps`. Tambem e lido pelo `reversa-pricing-advisor` para citar a comissao com fonte e data em vez de citar de memoria.

## Como ler este arquivo

Cada loja tem:

- `store`: chave canonica gravada em `advisor.store_choice.store` e em `product.models[].store`
- `nome`: nome amigavel usado no chat
- `commission_percent (padrao)`: percentual que o product usa sem perguntar
- `faixas e excecoes conhecidas`: situacoes em que o percentual real difere do padrao; aparecem no product.md como nota
- `source`: pagina oficial da loja consultada na data de referencia

## Disclaimer obrigatorio

Os percentuais aqui registrados sao uma aproximacao com base nas paginas oficiais conhecidas na data de referencia. Percentuais mudam por regiao, regulacao e faixa de receita. Validar na pagina oficial antes de decidir.

O agente deve repetir o disclaimer ao mostrar a comissao durante a entrevista e no rodape do `product.md`.

## Tabela

| store | nome | commission_percent (padrao) | faixas e excecoes conhecidas | source |
|---|---|---:|---|---|
| google_play | Google Play | 15 | 15% sobre o primeiro 1 milhao USD por ano por desenvolvedor, 30% acima; assinaturas 15%; pode variar por regiao e regulacao | Google Play Console Help, "Service fees": https://support.google.com/googleplay/android-developer/answer/112622 |
| app_store | Apple App Store | 30 | 15% no Small Business Program (ate 1 milhao USD por ano) e para assinaturas apos o primeiro ano; termos alternativos na UE | Apple Developer, App Store Small Business Program: https://developer.apple.com/app-store/small-business-program/ |
| microsoft_store | Microsoft Store (Windows) | 15 | 15% para apps, 12% para jogos; 0% quando o app usa sistema de pagamento proprio | Microsoft Learn, publicacao na Microsoft Store: https://learn.microsoft.com/en-us/windows/apps/publish/ |
| steam | Steam | 30 | 25% acima de 10 milhoes USD e 20% acima de 50 milhoes USD de receita acumulada do titulo | Steamworks Documentation, Steam Distribution Agreement: https://partner.steamgames.com/doc/gettingstarted/onboarding |
| outra | Outra loja | null | o usuario informa | nao se aplica |

## Como o product usa a tabela

1. Procura a linha de `advisor.store_choice.store`
2. Usa `commission_percent (padrao)` sem perguntar e mostra ao usuario o percentual, a fonte e a data de referencia
3. Copia as excecoes conhecidas para o product.md como nota do modelo
4. Se `store = outra`, ou a linha nao existir, pergunta "Qual a comissao da loja em percentual?"
5. Se a data de referencia desta tabela tiver mais de 12 meses em relacao a data de hoje conhecida pela sessao, oferece rodar o `reversa-pricing-research` com kind `store_commission` (opcional, S/N). Se o usuario aceitar e a pesquisa voltar com valor, mostra a faixa pesquisada (`faixa_min` a `faixa_max`) e pergunta qual percentual usar; Enter usa `faixa_max`, que e o percentual sem programa especial. O valor escolhido substitui o padrao e `commission_source` aponta para o arquivo de research

## Fontes oficiais

- Google Play Console Help, Service fees: https://support.google.com/googleplay/android-developer/answer/112622
- Apple Developer, App Store Small Business Program: https://developer.apple.com/app-store/small-business-program/
- Microsoft Learn, Publish Windows apps and games (Microsoft Store): https://learn.microsoft.com/en-us/windows/apps/publish/
- Steamworks Documentation, Onboarding e Steam Distribution Agreement: https://partner.steamgames.com/doc/gettingstarted/onboarding

## Como estender

1. Adicione uma linha na tabela com chave canonica em snake_case, sem espacos
2. Registre o percentual padrao sem programa especial ou adesao; programas de desconto entram como excecao
3. Liste as faixas e excecoes conhecidas em uma frase
4. Cite a pagina oficial da loja com URL
5. Atualize a data de referencia no topo do arquivo quando revalidar todos os percentuais
6. Nao chame o percentual de taxa contratual exata: e aproximacao para orcamento
7. Se a nova loja precisar virar opcao do advisor, acrescente a chave na pergunta "Em qual loja?" do `reversa-pricing-advisor`, mantendo a mesma grafia da coluna `store`
