# ADR-005: Lista duplamente encadeada de nós de comando e operador

- **Status:** Aceito (retroativo); substituiu a "cmd table"
- **Datas:** 2022-05-11 (`transforming linked list in doubly linkd list`), 2022-07-06 (PR #26 `refactor/list-without-cmd-table`, commit `refactor: change list logic`) 🟢

## Contexto
A primeira estrutura de execução era uma **tabela de comandos** (`char ***cmds`) dimensionada por `count_pipes * 2 + 3` (`creat_cmd_table`, removido em 2022-07-06). Ela dificultava associar redirecionamentos e pipes aos comandos.

## Decisão
Representar a linha como uma **lista duplamente encadeada** (`t_pipe_list`) na qual comandos e operadores são nós irmãos, na ordem em que aparecem. O nó de operador guarda a categoria em `args[0]` e o alvo em `args[1]`. Um `union { char *bin; int fd[2]; }` reaproveita memória entre os dois tipos de nó 🟢.

## Consequências
- ✅ Pipes e redirecionamentos são encontrados olhando os vizinhos (`prev`/`next`).
- ⚠️ A ligação comando↔redirecionamento depende da **posição**: só o `prev` imediato conta para a entrada, e só os nós seguintes contam para a saída. Isso gera os bugs RR-02 a RR-04 (redirecionamentos antes do comando). 🟢
- ⚠️ Argumentos depois de um redirecionamento (`cat < f -e`) viram um **novo nó de comando**. 🟢
- ⚠️ O union permite ler `fd` em um nó de comando (RR-07). 🟢
- ⚠️ O tipo do nó é uma string comparada por prefixo, então um comando chamado `PIPEx` seria tratado como operador. 🟢
