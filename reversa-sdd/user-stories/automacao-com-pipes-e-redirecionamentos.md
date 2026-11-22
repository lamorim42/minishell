# User Stories: Automação com Pipes e Redirecionamentos

> Redator (Reversa) · 2026-11-22
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA · ⚠️ DIVERGÊNCIA em relação ao bash
> Persona: **usuário que compõe comandos**, encadeando programas, gravando e lendo arquivos e alimentando programas com texto digitado. Não há modo não interativo (`-c`, script); toda "automação" acontece em linhas digitadas no prompt 🟢.

## US-11, Encadear programas
**Como** usuário, **quero** ligar a saída de um programa à entrada do próximo com `|`, **para** montar processamentos em etapas.

- Critérios:
  - `echo abc | cat | cat | wc -c` → `4` 🟢
  - Os comandos rodam em paralelo; `$?` é o status do último 🟢
  - `yes | head -1` termina 🟢
  - ⚠️ Um builtin no último nó roda no shell: `ls | exit 3` encerra o shell; `ls /nope | echo x` deixa `$?=2` (BUG-02, PL-03) 🟢
  - ⚠️ `ls |` é aceito e descarta a saída (RN-12) 🟢
- Units: `executar-pipeline`, `executar-comando-simples`
- Prioridade: Must

## US-12, Gravar a saída em arquivo
**Como** usuário, **quero** usar `>` e `>>`, **para** guardar resultados.

- Critérios:
  - `>` cria ou trunca; `>>` acrescenta; modo `0644` 🟢
  - O último redirecionamento vence; todos os arquivos são criados antes 🟢
  - O redirecionamento vence o pipe (`echo a > f | cat` → `cat` vazio) 🟢
  - ⚠️ `> f cmd` (redirecionamento antes do comando) é ignorado (BUG-08) 🟢
  - ⚠️ `echo a > f b` executa `b` como comando (ML-04) 🟢
  - ⚠️ Destino impossível (`> /nonexistent/x`) derruba o shell (BUG-01) 🟢
  - ⚠️ Não há `2>` (stderr) 🟢
- Units: `redirecionar-entrada-e-saida`, `executar-comando-simples`
- Prioridade: Must

## US-13, Ler a entrada de um arquivo
**Como** usuário, **quero** usar `<`, **para** alimentar programas com arquivos.

- Critérios:
  - `cat < f` e `< f cat` funcionam 🟢
  - `cat < nope` → `nope: No such file or directory`, `$?=1` 🟢
  - ⚠️ `< nope cat` roda sem erro (BUG-09) 🟢
  - ⚠️ `ls | > o wc` não lê o pipe e espera o terminal (RR-04) 🟢
- Units: `redirecionar-entrada-e-saida`
- Prioridade: Must

## US-14, Fornecer texto digitado (heredoc)
**Como** usuário, **quero** usar `<< DELIM`, **para** passar várias linhas a um programa sem criar arquivo.

- Critérios:
  - Prompt `> ` até a linha igual ao delimitador; o delimitador perde as aspas 🟢
  - Vários heredocs na mesma linha são lidos em ordem; o último vence 🟢
  - Combina com `|` e com `>` 🟢
  - EOF → `<DELIM>: Wanted` e o conteúdo parcial é usado 🟢
  - ⚠️ O conteúdo nunca é expandido (RN-27) 🟢
  - ⚠️ Ctrl-C não cancela o comando (BUG-15) 🟢
  - ⚠️ Conteúdo maior que ~64 KiB trava o shell (RE-09) 🟢
- Units: `ler-heredoc`, `tratar-sinais`
- Prioridade: Must

## US-15, Usar variáveis em comandos compostos
**Como** usuário, **quero** usar `$VAR` e `$?` dentro de pipelines e argumentos, **para** parametrizar comandos.

- Critérios:
  - `cd $HOME/x`, `echo "$HOME $USER"`, `echo pre$HOME/pos` expandem 🟢
  - `$?` reflete o último pipeline 🟢
  - ⚠️ `> $F`, `< $F` e `<< $EOF` são erro de sintaxe (BUG-13) 🟢
  - ⚠️ Sem divisão de palavras: `V='a b'; ls $V` passa um argumento só (RN-25) 🟢
  - ⚠️ Programas filhos não veem `export` (BUG-03) 🟢
- Units: `expandir-variaveis`, `redirecionar-entrada-e-saida`, `gerenciar-variaveis-de-ambiente`
- Prioridade: Should

## Lacunas
- ✅ L-01 (respondida em 2026-11-22): o alvo é o **mínimo exigido pelo subject** da 42; o modo não interativo (`./minishell -c`, script) fica **fora do escopo**
- ✅ L-07 (decidida em 2026-11-22): documentar o comportamento atual; as user stories acima descrevem o comportamento especificado
