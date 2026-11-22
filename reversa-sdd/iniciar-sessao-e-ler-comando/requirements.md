# Iniciar Sessão e Ler Comando

> Unit de caso de uso · Redator (Reversa) · 2026-11-11
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash

## Visão Geral
Inicializa o shell (estado da linha e ambiente) e mantém o laço interativo **ler → filtrar → despachar**. Cada linha digitada no prompt `miau> ` é verificada (EOF, aspas, comentário) antes de seguir para o processamento (`facade`). É a porta de entrada de todos os outros casos de uso 🟢.

## Responsabilidades
- Inicializar a `t_line` e copiar o `envp` para a tabela de ambiente, incluindo `?=0` 🟢
- Guardar cópias de stdin e stdout e restaurá-las depois de cada linha 🟢
- Mostrar o prompt e ler a linha com GNU Readline 🟢
- Encerrar a sessão com Ctrl-D (EOF) 🟢
- Rejeitar linhas com aspas desbalanceadas 🟢
- Ignorar linhas de comentário 🟢
- Despachar as outras linhas para a Facade 🟢
- Registrar no histórico as linhas que foram executadas 🟢

## Regras de Negócio
- RN-01: o prompt é exatamente `miau> ` 🟢
- RN-02: EOF no prompt → escreve `exit\n` em **stderr** e termina com código **0** 🟢. ⚠️ O bash termina com o último `$?`. Bug menor: um byte `\0` extra é escrito (BUG-19)
- RN-03: aspas desbalanceadas → `Syntax ERROR!\n` em **stdout**, a linha é descartada e `$?` não muda 🟢. ⚠️ O bash pediria continuação (PS2)
- RN-04: se o primeiro caractere depois dos espaços iniciais (só `' '`) for `#`, a linha inteira é ignorada 🟢
- RN-03 é avaliada **antes** da RN-04: `# it's` gera erro de sintaxe 🟢
- RN-05: só entram no histórico as linhas que chegam à execução (sintaxe válida e não vazia). ⚠️ O bash registra toda linha não vazia 🟢
- Linha vazia (`""`) é descartada sem efeito. Linha só com espaços percorre o pipeline inteiro e entra no histórico 🟢
- Argumentos passados ao `./minishell` são ignorados; não há modo não interativo (`-c`, script) 🟢

## Requisitos Funcionais

| ID | Requisito | Prioridade | Critério de Aceite |
|----|-----------|-----------|-------------------|
| RF-01 | Carregar todas as entradas `K=V` do `envp` na tabela, preservando `=` dentro do valor, e acrescentar `?`=`0` | Must | `env` lista as variáveis herdadas e `?=0` |
| RF-02 | Mostrar `miau> ` e ler uma linha por iteração | Must | O prompt aparece depois de cada comando |
| RF-03 | Com EOF, liberar a tabela, escrever `exit` em stderr e sair com 0 | Must | Ctrl-D encerra o processo, `echo $?` no shell pai mostra 0 |
| RF-04 | Rejeitar aspas desbalanceadas com `Syntax ERROR!` | Must | `echo "abc` → mensagem, nada executa |
| RF-05 | Ignorar comentários iniciados por `#` | Should | `   # nada` → nenhuma saída |
| RF-06 | Restaurar stdin e stdout originais depois de cada linha | Must | Depois de `ls > f`, o próximo `ls` sai no terminal |
| RF-07 | Adicionar ao histórico as linhas executadas | Should | Seta para cima recupera o último comando executado |

## Requisitos Não Funcionais

| Tipo | Requisito inferido | Evidência no código | Confiança |
|------|--------------------|---------------------|-----------|
| Interatividade | Edição de linha e histórico em memória via GNU Readline; o histórico não é salvo em arquivo | `src/main.c:45`, `src/facade.c:57` | 🟢 |
| Recursos | Um par de fds (`dup` de 0 e 1) mantido aberto durante toda a sessão | `src/main.c:40-41` | 🟢 |

## Critérios de Aceitação

```gherkin
Funcionalidade: Sessão interativa

  Cenário: Prompt e execução de linha válida
    Dado que o minishell foi iniciado
    Quando o usuário digita "echo oi"
    Então "oi" é exibido
    E o prompt "miau> " volta a aparecer

  Cenário: Encerrar com Ctrl-D
    Dado o prompt vazio
    Quando o usuário envia EOF (Ctrl-D)
    Então "exit" é escrito em stderr
    E o processo termina com código 0

  Cenário: Aspas desbalanceadas
    Dado o prompt
    Quando o usuário digita echo "abc
    Então "Syntax ERROR!" é escrito em stdout
    E nenhum comando é executado
    E o valor de $? não muda

  Cenário: Comentário
    Quando o usuário digita "   # comentário"
    Então nada é executado nem exibido

  Cenário: Restauração de stdout
    Dado que o usuário executou "echo a > f"
    Quando o usuário digita "echo b"
    Então "b" aparece no terminal

  @divergente
  Cenário: Ctrl-D depois de um erro (bash usaria o último status)
    Dado que o último comando terminou com status 1
    Quando o usuário envia EOF
    Então o processo termina com código 0
```

## Prioridade (MoSCoW)

| Requisito | MoSCoW | Justificativa |
|-----------|--------|---------------|
| RF-01, RF-02, RF-03, RF-06 | Must | Sem eles não há sessão utilizável |
| RF-04 | Must | Evita tokenizar entrada inválida |
| RF-05, RF-07 | Should | Conveniências |

## Rastreabilidade de Código

| Arquivo | Função | Cobertura |
|---------|--------|-----------|
| `src/main.c` | `main`, `main_loop`, `control_d`, `is_a_comment`, `file_exists` | 🟢 |
| `src/init.c` | `init_line` | 🟢 |
| `src/hash_table/create_table.c` | `population_hash_table`, `preper_to_insert` | 🟢 |
| `src/tokens/sintax_analysis.c` | `verify_quots`, `find_pair` | 🟢 |
| `src/fd_handler_utils.c` | `close_std_fd` | 🟢 |
