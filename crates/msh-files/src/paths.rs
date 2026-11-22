//! Caminhos: validação, nomes únicos e o comando `cd` que a GUI digita no
//! terminal (TDD §7, "tela → shell").

use std::io;
use std::path::{Component, Path, PathBuf};

/// Um caminho absoluto e já normalizado: começa em `/` e não tem `.`, `..`
/// nem barras repetidas. O helper de administrador só aceita caminhos
/// assim, para que o que o polkit mostra seja exatamente o que é tocado.
///
/// # Errors
///
/// `InvalidInput` se o caminho for relativo ou tiver `.`/`..`.
pub fn check_absolute(path: &Path) -> io::Result<()> {
    let bytes = path.as_os_str().as_encoded_bytes();
    let normal = path.is_absolute()
        && !bytes.windows(2).any(|w| w == b"//")
        && (bytes.len() == 1 || !bytes.ends_with(b"/"))
        && path
            .components()
            .all(|c| matches!(c, Component::RootDir | Component::Normal(_)))
        && !bytes.split(|&b| b == b'/').any(|s| s == b"." || s == b"..");
    if normal {
        Ok(())
    } else {
        Err(invalid(format!(
            "caminho precisa ser absoluto e normalizado: {}",
            path.display()
        )))
    }
}

/// Um nome de arquivo válido para renomear: não vazio, sem `/` e sem NUL, e
/// diferente de `.` e `..`.
///
/// # Errors
///
/// `InvalidInput` com uma mensagem para o usuário.
pub fn check_name(name: &str) -> io::Result<()> {
    if name.is_empty() {
        Err(invalid("o nome não pode ficar vazio".into()))
    } else if name == "." || name == ".." {
        Err(invalid(format!("“{name}” não é um nome válido")))
    } else if name.contains('/') || name.contains('\0') {
        Err(invalid("o nome não pode ter “/”".into()))
    } else {
        Ok(())
    }
}

/// Um nome livre em `dir` para uma cópia de `name`: o próprio nome, se não
/// existir, ou `nome (cópia).ext`, `nome (cópia 2).ext`, ...
#[must_use]
pub fn unique_name(dir: &Path, name: &str, exists: impl Fn(&Path) -> bool) -> String {
    if !exists(&dir.join(name)) {
        return name.to_owned();
    }
    // A extensão só conta se não for o nome todo (".bashrc" não tem).
    let (stem, ext) = match name.rfind('.') {
        Some(i) if i > 0 => name.split_at(i),
        _ => (name, ""),
    };
    // Com 2^32 cópias já existentes, desiste e sobrescreve a última; na
    // prática nunca chega lá.
    let candidate = |n: u32| {
        if n == 1 {
            format!("{stem} (cópia){ext}")
        } else {
            format!("{stem} (cópia {n}){ext}")
        }
    };
    (1..u32::MAX)
        .map(candidate)
        .find(|c| !exists(&dir.join(c)))
        .unwrap_or_else(|| candidate(u32::MAX))
}

/// `true` se `inner` for `outer` ou estiver dentro dele (sem seguir links).
#[must_use]
pub fn is_within(inner: &Path, outer: &Path) -> bool {
    inner.starts_with(outer)
}

/// O comando que a GUI digita no terminal para mudar o diretório do shell:
/// `cd 'caminho'`, com a aspa simples escapada como `'\''` (POSIX 2.2.2).
/// O caminho é sempre absoluto, então não vira opção do `cd`.
#[must_use]
pub fn cd_command(path: &Path) -> String {
    format!("cd {}\n", shell_quote(&path.to_string_lossy()))
}

/// Coloca `s` entre aspas simples para o shell.
#[must_use]
pub fn shell_quote(s: &str) -> String {
    format!("'{}'", s.replace('\'', r"'\''"))
}

/// O diretório pai, ou o próprio `/`.
#[must_use]
pub fn parent_or_root(path: &Path) -> PathBuf {
    path.parent()
        .map_or_else(|| PathBuf::from("/"), Path::to_path_buf)
}

fn invalid(msg: String) -> io::Error {
    io::Error::new(io::ErrorKind::InvalidInput, msg)
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::HashSet;

    #[test]
    fn absolute_paths() {
        for ok in ["/", "/etc", "/etc/hosts", "/home/ana/meus arquivos"] {
            assert!(check_absolute(Path::new(ok)).is_ok(), "{ok}");
        }
        for bad in [
            "",
            "etc",
            "/etc/",
            "/etc/../root",
            "/./etc",
            "//etc",
            "/a//b",
        ] {
            assert!(check_absolute(Path::new(bad)).is_err(), "{bad}");
        }
    }

    #[test]
    fn names() {
        assert!(check_name("nota.txt").is_ok());
        assert!(check_name(".bashrc").is_ok());
        for bad in ["", ".", "..", "a/b", "a\0b"] {
            assert!(check_name(bad).is_err(), "{bad:?}");
        }
    }

    #[test]
    fn unique_names() {
        let taken: HashSet<_> = ["/d/a.txt", "/d/a (cópia).txt", "/d/.bashrc", "/d/pasta"]
            .into_iter()
            .map(PathBuf::from)
            .collect();
        let exists = |p: &Path| taken.contains(p);
        let dir = Path::new("/d");
        assert_eq!(unique_name(dir, "b.txt", exists), "b.txt");
        assert_eq!(unique_name(dir, "a.txt", exists), "a (cópia 2).txt");
        assert_eq!(unique_name(dir, ".bashrc", exists), ".bashrc (cópia)");
        assert_eq!(unique_name(dir, "pasta", exists), "pasta (cópia)");
    }

    #[test]
    fn cd_is_quoted() {
        assert_eq!(cd_command(Path::new("/tmp")), "cd '/tmp'\n");
        assert_eq!(
            cd_command(Path::new("/tmp/it's $HOME")),
            "cd '/tmp/it'\\''s $HOME'\n"
        );
    }

    #[test]
    fn within() {
        assert!(is_within(Path::new("/a/b"), Path::new("/a")));
        assert!(is_within(Path::new("/a"), Path::new("/a")));
        assert!(!is_within(Path::new("/ab"), Path::new("/a")));
    }
}
