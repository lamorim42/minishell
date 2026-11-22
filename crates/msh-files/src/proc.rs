//! O shell do terminal está livre no prompt?
//!
//! A GUI só digita `cd '...'` no terminal quando o minishell não está rodando
//! nada: com um `cat` ou um `vim` em primeiro plano, o texto iria para o
//! programa, não para o shell. O minishell não tem job control, então os
//! filhos ficam no mesmo grupo de processos e o `tcgetpgrp` do PTY não ajuda;
//! em vez disso, procuramos filhos do shell em `/proc`.

use std::path::Path;

/// `true` se este processo roda como root. A GUI se recusa a abrir assim
/// (PRD §3, "GUI sem root"): ações de root passam pelo helper do polkit.
#[must_use]
pub fn running_as_root() -> bool {
    nix::unistd::geteuid().is_root()
}

/// `true` se o processo `pid` tem algum filho vivo, segundo `proc_root`
/// (normalmente `/proc`).
#[must_use]
pub fn has_children(proc_root: &Path, pid: u32) -> bool {
    // Caminho rápido: /proc/<pid>/task/<pid>/children (CONFIG_PROC_CHILDREN,
    // ligado no kernel do Debian). Como o shell tem uma thread só, a lista
    // da thread principal basta.
    let children = proc_root.join(format!("{pid}/task/{pid}/children"));
    if let Ok(text) = std::fs::read_to_string(children) {
        return !text.trim().is_empty();
    }
    // Sem esse arquivo, percorre /proc/*/stat procurando o ppid.
    let Ok(entries) = std::fs::read_dir(proc_root) else {
        return false;
    };
    entries.flatten().any(|e| {
        e.file_name()
            .to_str()
            .is_some_and(|n| n.bytes().all(|b| b.is_ascii_digit()))
            && std::fs::read_to_string(e.path().join("stat"))
                .ok()
                .and_then(|stat| parent_pid(&stat))
                == Some(pid)
    })
}

/// O ppid (4º campo) de uma linha de `/proc/<pid>/stat`. O nome do comando,
/// entre parênteses, pode ter espaços e `)`, então contamos a partir do
/// último `)`.
#[must_use]
pub fn parent_pid(stat: &str) -> Option<u32> {
    let after = &stat[stat.rfind(')')? + 1..];
    after.split_whitespace().nth(1)?.parse().ok()
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::process::Command;

    #[test]
    fn parses_ppid_even_with_odd_names() {
        assert_eq!(parent_pid("42 (bash) S 7 42 42 0"), Some(7));
        assert_eq!(parent_pid("42 (a) b) c) R 9 1 1"), Some(9));
        assert_eq!(parent_pid("lixo"), None);
    }

    #[test]
    fn sees_a_running_child() {
        // O `sh` (uma thread só, como o minishell) roda o `sleep` num filho;
        // o `; :` impede que ele troque de imagem com exec.
        let mut sh = Command::new("sh")
            .args(["-c", "sleep 5; :"])
            .spawn()
            .unwrap();
        let found = (0..100).any(|_| {
            std::thread::sleep(std::time::Duration::from_millis(20));
            has_children(Path::new("/proc"), sh.id())
        });
        sh.kill().unwrap();
        sh.wait().unwrap();
        assert!(found, "o sleep deveria aparecer como filho do sh");

        let fake = std::env::temp_dir().join(format!("msh-proc-{}", std::process::id()));
        std::fs::create_dir_all(fake.join("1/task/1")).unwrap();
        std::fs::write(fake.join("1/task/1/children"), "").unwrap();
        assert!(!has_children(&fake, 1));
        std::fs::write(fake.join("1/task/1/children"), "77 ").unwrap();
        assert!(has_children(&fake, 1));
        std::fs::remove_dir_all(fake).unwrap();
    }

    #[test]
    fn falls_back_to_stat() {
        let fake = std::env::temp_dir().join(format!("msh-proc-stat-{}", std::process::id()));
        std::fs::create_dir_all(fake.join("10")).unwrap();
        std::fs::write(fake.join("10/stat"), "10 (cat) S 5 10 10 0").unwrap();
        assert!(has_children(&fake, 5));
        assert!(!has_children(&fake, 6));
        std::fs::remove_dir_all(fake).unwrap();
    }
}
