# C4 Nível 3: Componentes (processo principal)

> Gerado pelo **Arquiteto** (Reversa) em 2026-11-11

```mermaid
flowchart LR
    subgraph core["core"]
        REPL["REPL<br/><i>main.c</i>"]
        FAC["Facade<br/><i>facade.c</i>"]
        MEM["Memória e erros<br/><i>free.c, init.c, arr_utils.c</i>"]
    end
    subgraph tokens["tokens (parser)"]
        CNT["Contador<br/><i>count_tks.c</i>"]
        TOK["Tokenizer<br/><i>tokenizer.c</i>"]
        LEX["Analisador léxico<br/><i>lexical_analyzer.c</i>"]
        SYN["Analisador sintático e aspas<br/><i>sintax_analysis.c</i>"]
        CLN["Removedor de aspas<br/><i>clean_tokens.c</i>"]
    end
    subgraph exec["exec"]
        EXP["Expansor de variáveis<br/><i>expand_var*.c</i>"]
        LST["Montador da lista<br/><i>creat_cmd_table.c, make_list.c</i>"]
        HD["Heredoc<br/><i>here_doc*.c</i>"]
        RES["Resolvedor de binário<br/><i>list_generation_bin.c, path_finder.c</i>"]
        RUN["Executor<br/><i>exec_list.c, init_fork.c, exec_path.c</i>"]
    end
    subgraph redir["redirections"]
        FDO["Abertura de fds<br/><i>fd_handler_utils.c</i>"]
        FDL["Ligação de fds<br/><i>fd_handler.c</i>"]
    end
    subgraph bi["builtins"]
        DSP["Despachante<br/><i>builtins_utils.c</i>"]
        BIS["echo · cd · pwd · export<br/>unset · env · exit"]
    end
    subgraph env["hash_table"]
        HT["Ambiente<br/><i>insert · search · delete</i>"]
    end
    subgraph sig["signals"]
        SG["Handlers<br/><i>signals*.c</i>"]
    end
    G[("g_minishell<br/>global")]

    REPL --> SYN
    REPL --> FAC
    REPL --> SG
    FAC --> CNT & TOK & LEX
    FAC --> SYN
    FAC --> EXP --> HT
    FAC --> CLN
    FAC --> LST
    FAC --> HD
    FAC --> RES --> HT
    RES --> DSP
    FAC --> FDO
    FAC --> RUN
    RUN --> FDL
    RUN --> DSP --> BIS --> HT
    RUN --> SG
    RUN -- "$?" --> HT
    SG -.-> G
    BIS -.-> G
    G -.-> MEM
```

## Responsabilidades

| Componente | Entradas | Saídas | Observações |
|------------|----------|--------|-------------|
| REPL | stdin | `line.str` | Faz a checagem de aspas **antes** da Facade |
| Facade | `line.str` | efeitos colaterais | Ordem fixa dos filtros (ADR-001) |
| Contador + Tokenizer | `str` | `tks_nbr`, `tks` | Duas implementações das mesmas regras (ADR-003) |
| Analisador léxico | `tks` | `lex` | Categorias em forma de string |
| Analisador sintático | `lex` | bool | Também contém `verify_quots` |
| Expansor | `tks`, `lex`, tabela | `tks` alterado | Roda **antes** da remoção de aspas |
| Removedor de aspas | `tks` | `ctks` | |
| Montador da lista | `ctks`, `lex` | `list_cmds` | ADR-005 |
| Heredoc | nós `HERE` | conteúdo nos pipes | ADR-006, em processo filho |
| Resolvedor | nós de comando, tabela (`PATH`) | `node->bin` | Builtin detectado por prefixo |
| Abertura de fds | nós de operador | `node->fd` | Roda no pai, antes dos forks |
| Executor | lista | processos, `$?` | ADR-009 |
| Ligação de fds | nó de comando | `dup2` | Roda no filho |
| Builtins | nó | stdout, tabela | Usa a global para gravar o status |
| Ambiente | chave/valor | — | ADR-004 |
| Sinais | sinal | `\n`, status, exit | Usa a global (ADR-007) |
