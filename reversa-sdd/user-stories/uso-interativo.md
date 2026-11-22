# User Stories: Uso Interativo

> Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash
> Persona: **usuário de terminal** que usa o minishell como um bash simplificado no dia a dia (navegar, rodar programas, ajustar variáveis). Não há papéis nem permissões próprias (ver `permissions.md`) 🟢.

## US-01, Iniciar e encerrar o shell
**Como** usuário de terminal, **quero** abrir o minishell, ver um prompt e sair quando terminar, **para** trabalhar em uma sessão isolada.

- Critérios:
  - O prompt é `miau> ` 🟢
  - Ctrl-D no prompt vazio escreve `exit` e encerra com 0 🟢 ⚠️ (o bash usa o último `$?`)
  - `exit n` encerra com `n & 0xFF`; `exit abc` encerra com 2; `exit 1 2` não encerra 🟢
- Units: `iniciar-sessao-e-ler-comando`, `encerrar-shell`
- Prioridade: Must

## US-02, Executar programas
**Como** usuário, **quero** digitar o nome de um programa com argumentos e vê-lo rodar, **para** usar as ferramentas do sistema.

- Critérios:
  - Nomes sem `/` são procurados no `PATH`; `/`, `./` e `../` usam o caminho literal 🟢
  - Comando inexistente → `<nome>: command not found` e `$?=127` 🟢
  - `$?` mostra o status do último comando 🟢
  - ⚠️ Nomes vazios ou prefixos de `PIPE`/`REDO`/`REDA`/`REDI`/`HERE` (`P`, `R`, `$NOPE ls`) são ignorados em silêncio (BUG-20) 🟢
  - ⚠️ Programas com prefixo de builtin (`pwdx`, `envsubst`) não executam (BUG-04) 🟢
- Units: `executar-comando-simples`, `analisar-linha-de-comando`
- Prioridade: Must

## US-03, Navegar entre diretórios
**Como** usuário, **quero** mudar de diretório e saber onde estou, **para** trabalhar com arquivos em lugares diferentes.

- Critérios:
  - `cd <dir>`, `cd` e `cd ~` funcionam e atualizam `PWD`/`OLDPWD` 🟢
  - `cd /nope` → mensagem e `$?=1` 🟢
  - ⚠️ `cd -` e `cd ~/x` não funcionam; `pwd` mostra a variável `PWD`, não o diretório real 🟢
- Units: `navegar-diretorios`
- Prioridade: Must

## US-04, Usar variáveis de ambiente
**Como** usuário, **quero** criar, ler e remover variáveis, **para** configurar a sessão e reaproveitar valores.

- Critérios:
  - `export K=V`, `unset K` e `env` alteram e mostram a tabela do shell 🟢
  - `$K` e `"$K"` expandem; `'$K'` não 🟢
  - ⚠️ As variáveis exportadas **não** chegam aos programas filhos (BUG-03, L-03) 🟢
  - ⚠️ `export A=b=c` grava `b=b=`; `export A=` é ignorado (BUG-06) 🟢
  - ⚠️ Variável inexistente apaga o token inteiro (`"a $NOPE b"` → vazio) (BUG-05) 🟢
  - ⚠️ Um valor que se refere a si mesmo (`A='$A'` herdado) trava o shell (RX-06) 🟢
- Units: `gerenciar-variaveis-de-ambiente`, `expandir-variaveis`
- Prioridade: Must

## US-05, Imprimir texto
**Como** usuário, **quero** usar `echo` com e sem `-n`, **para** mostrar mensagens e montar arquivos.

- Critérios:
  - `echo a b` → `a b`; `echo -n x` sem quebra de linha 🟢
  - Aspas simples e duplas agrupam palavras e são removidas 🟢
  - ⚠️ `"a"b"c"` vira `a"b"c` (BUG-11); `echo $X'q'` gera dois argumentos (BUG-17) 🟢
- Units: `imprimir-texto-echo`, `analisar-linha-de-comando`
- Prioridade: Must

## US-06, Interromper comandos com o teclado
**Como** usuário, **quero** que Ctrl-C e Ctrl-\ interrompam o que está rodando sem fechar o shell, **para** recuperar o controle.

- Critérios:
  - Ctrl-C no prompt mostra um prompt novo; Ctrl-\ é ignorado 🟢
  - Ctrl-C durante um comando → `$?=130`; Ctrl-\ → `Quit` e `$?=131` 🟢
  - ⚠️ Ctrl-C no prompt não grava `$?=130` (SG-02) 🟢
  - ⚠️ Ctrl-C no heredoc não cancela o comando (BUG-15) 🟢
- Units: `tratar-sinais`, `ler-heredoc`
- Prioridade: Must

## US-07, Recuperar comandos anteriores
**Como** usuário, **quero** usar a seta para cima para repetir comandos, **para** não redigitar.

- Critérios:
  - O histórico do readline guarda as linhas executadas, só em memória 🟢
  - ⚠️ Linhas com erro de sintaxe ou aspas abertas não entram no histórico (RN-05) 🟢
- Units: `iniciar-sessao-e-ler-comando`
- Prioridade: Should

## US-08, Ser avisado de erros de digitação
**Como** usuário, **quero** que o shell recuse linhas mal formadas, **para** não executar algo errado.

- Critérios:
  - Aspas abertas, `|` no início, `||` e redirecionamento sem alvo → `Syntax ERROR!` e nada executa 🟢
  - ⚠️ A mensagem vai para stdout e `$?` não muda (L-04) 🟢
  - ⚠️ `ls |` é aceito sem aviso (RN-12); `> $F` é recusado (BUG-13) 🟢
- Units: `analisar-linha-de-comando`, `iniciar-sessao-e-ler-comando`
- Prioridade: Must

## Lacunas
- ✅ L-01 (respondida em 2026-11-22): o alvo é o **mínimo exigido pelo subject** da 42; as ⚠️ que tratam de recursos além do subject (`${}`, `$$`, `cd -`, `-c`, tab) são **fora do escopo**; as demais são bugs conhecidos (L-02)
- ✅ L-07 (decidida em 2026-11-22): documentar o comportamento atual; as user stories acima descrevem o comportamento especificado
