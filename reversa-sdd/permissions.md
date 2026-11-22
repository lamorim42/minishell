# Permissões — minishell

> Gerado pelo **Detetive** (Reversa) em 2026-11-11
> Escala: 🟢 CONFIRMADO · 🟡 INFERIDO · 🔴 LACUNA

## 1. Papéis e RBAC

**Não se aplica.** O minishell não tem usuários, autenticação, papéis nem ACL próprios 🟢. Ele roda com a identidade (UID/GID) de quem o executou, e todo controle de acesso fica com o sistema operacional.

## 2. Matriz de interação com permissões do sistema operacional

| Operação | O que o minishell faz | Checagem própria | Confiança |
|----------|-----------------------|------------------|-----------|
| Executar programa pelo nome | Procura no `PATH` com `access(path, F_OK)` | Só **existência**, não permissão de execução. Um arquivo sem `+x` é escolhido e falha no `execve` (`Permission denied`, `$?=127`) | 🟢 |
| Executar programa pelo caminho | `execve` direto | Nenhuma. Falha → `perror` e 127 | 🟢 |
| Criar arquivo com `>` / `>>` | `open(..., O_CREAT, 0644)` | O modo final depende da `umask` do processo | 🟢 |
| Ler arquivo com `<` | `open(O_RDONLY)` | Falha com `-1`; a mensagem genérica é `No such file or directory`, **mesmo quando o erro real é de permissão** | 🟢 |
| `cd` | `chdir` | `perror` mostra a causa real (por exemplo `Permission denied`) | 🟢 |
| Ambiente | Herdado do processo pai, mas `export`/`unset` não chegam aos filhos | — | 🟢 |

## 3. Observações de segurança

- **Sem escalonamento:** não há `setuid`, `sudo` nem `chmod` 🟢.
- **Crash em erro de I/O:** um caminho de saída inválido derruba o shell (BUG-01). Para um shell interativo isso é perda de sessão, não uma vulnerabilidade explorável por terceiros 🟡.
- **Handlers de sinal não *async-signal-safe*:** `printf`, `free` e funções do readline dentro de handlers podem corromper o estado se o sinal chegar no meio de um `malloc`. É raro, mas possível 🟡.
- **`PATH` vazio ou com `.`:** como a busca usa `F_OK`, um `PATH` que contém `.` executaria arquivos do diretório atual, igual ao bash 🟡.
