//! O helper de administrador (RF-24, TDD §7).
//!
//! A GUI nunca roda como root. Quando uma ação precisa de root (por exemplo,
//! salvar um arquivo em `/etc`), ela chama
//! `pkexec /usr/libexec/miau-files/miau-files-helper <ação> <args>`. O
//! polkit pede a senha de administrador (ação `org.miaushell.files.helper`)
//! e só então o helper roda, como root, a única ação pedida.
//!
//! Para que a autorização valha exatamente para o que o usuário viu, o
//! helper é estrito: aceita poucas ações, só caminhos absolutos e
//! normalizados, nunca segue um link no alvo de `write` e não lê nada além
//! dos argumentos e do stdin.

use std::io::{self, Read};
use std::path::{Path, PathBuf};

use crate::{ops, paths};

/// Uma ação pedida ao helper.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Action {
    /// `copy ORIGEM PASTA`
    Copy { src: PathBuf, dest_dir: PathBuf },
    /// `move ORIGEM PASTA`
    Move { src: PathBuf, dest_dir: PathBuf },
    /// `rename CAMINHO NOVO_NOME`
    Rename { path: PathBuf, new_name: String },
    /// `mkdir CAMINHO`
    MakeDir { path: PathBuf },
    /// `write CAMINHO` (o conteúdo vem pelo stdin)
    Write { path: PathBuf },
}

pub const USAGE: &str = "usage: miau-files-helper copy SRC DIR | move SRC DIR | rename PATH NAME | mkdir PATH | write PATH < content";

impl Action {
    /// Lê a ação dos argumentos (sem o nome do programa).
    ///
    /// # Errors
    ///
    /// Uma mensagem quando a ação não existe, os argumentos não batem ou um
    /// caminho não é absoluto e normalizado.
    pub fn parse(args: &[String]) -> Result<Self, String> {
        let abs = |s: &String| -> Result<PathBuf, String> {
            let p = PathBuf::from(s);
            paths::check_absolute(&p).map_err(|e| e.to_string())?;
            Ok(p)
        };
        let action = match args {
            [a, src, dir] if a == "copy" => Self::Copy {
                src: abs(src)?,
                dest_dir: abs(dir)?,
            },
            [a, src, dir] if a == "move" => Self::Move {
                src: abs(src)?,
                dest_dir: abs(dir)?,
            },
            [a, path, name] if a == "rename" => {
                paths::check_name(name).map_err(|e| e.to_string())?;
                Self::Rename {
                    path: abs(path)?,
                    new_name: name.clone(),
                }
            }
            [a, path] if a == "mkdir" => Self::MakeDir { path: abs(path)? },
            [a, path] if a == "write" => Self::Write { path: abs(path)? },
            _ => return Err(USAGE.to_owned()),
        };
        if let Self::Copy { src, .. } | Self::Move { src, .. } | Self::Rename { path: src, .. } =
            &action
            && src == Path::new("/")
        {
            return Err("a raiz não pode ser copiada, movida ou renomeada".to_owned());
        }
        Ok(action)
    }

    /// Executa a ação. Devolve o caminho que ela criou ou mudou.
    ///
    /// # Errors
    ///
    /// Os erros de [`ops`].
    pub fn run(&self, stdin: impl Read) -> io::Result<PathBuf> {
        match self {
            Self::Copy { src, dest_dir } => ops::copy_into(src, dest_dir),
            Self::Move { src, dest_dir } => ops::move_into(src, dest_dir),
            Self::Rename { path, new_name } => ops::rename(path, new_name),
            Self::MakeDir { path } => ops::make_dir(path).map(|()| path.clone()),
            Self::Write { path } => {
                let content = ops::read_limited(stdin)?;
                ops::write_file(path, &content).map(|()| path.clone())
            }
        }
    }

    /// Os argumentos para chamar o helper com esta ação (o inverso de
    /// [`Action::parse`]); a GUI usa isto para montar a linha do `pkexec`.
    #[must_use]
    pub fn to_args(&self) -> Vec<String> {
        let s = |p: &Path| p.to_string_lossy().into_owned();
        match self {
            Self::Copy { src, dest_dir } => vec!["copy".into(), s(src), s(dest_dir)],
            Self::Move { src, dest_dir } => vec!["move".into(), s(src), s(dest_dir)],
            Self::Rename { path, new_name } => {
                vec!["rename".into(), s(path), new_name.clone()]
            }
            Self::MakeDir { path } => vec!["mkdir".into(), s(path)],
            Self::Write { path } => vec!["write".into(), s(path)],
        }
    }
}

/// O programa `miau-files-helper`: 0 se deu certo (e o caminho no stdout),
/// 1 se a ação falhou, 2 para uso errado.
pub fn main_with(args: &[String], stdin: impl Read) -> i32 {
    let action = match Action::parse(args) {
        Ok(a) => a,
        Err(msg) => {
            eprintln!("miau-files-helper: {msg}");
            return 2;
        }
    };
    match action.run(stdin) {
        Ok(path) => {
            println!("{}", path.display());
            0
        }
        Err(e) => {
            eprintln!("miau-files-helper: {}", crate::format::io_error(&e));
            1
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn args(list: &[&str]) -> Vec<String> {
        list.iter().map(|&s| s.to_owned()).collect()
    }

    #[test]
    fn parses_and_round_trips() {
        for a in [
            Action::Copy {
                src: "/etc/hosts".into(),
                dest_dir: "/root".into(),
            },
            Action::Move {
                src: "/srv/a b".into(),
                dest_dir: "/srv/c".into(),
            },
            Action::Rename {
                path: "/etc/x.conf".into(),
                new_name: "y.conf".into(),
            },
            Action::MakeDir {
                path: "/opt/novo".into(),
            },
            Action::Write {
                path: "/etc/hosts".into(),
            },
        ] {
            assert_eq!(Action::parse(&a.to_args()), Ok(a));
        }
    }

    #[test]
    fn strict_about_input() {
        for bad in [
            &[][..],
            &["rm", "/etc"][..],
            &["write"][..],
            &["write", "etc/hosts"][..],
            &["write", "/etc/../root/.ssh"][..],
            &["write", "/etc/hosts", "extra"][..],
            &["copy", "/etc/hosts", "tmp"][..],
            &["rename", "/etc/hosts", "../passwd"][..],
            &["move", "/", "/tmp"][..],
        ] {
            assert!(Action::parse(&args(bad)).is_err(), "{bad:?}");
        }
    }

    #[test]
    fn runs_write_from_stdin() {
        let dir = std::env::temp_dir().join(format!("msh-helper-{}", std::process::id()));
        std::fs::create_dir_all(&dir).unwrap();
        let file = dir.join("conf");
        let code = main_with(&args(&["write", file.to_str().unwrap()]), &b"ok\n"[..]);
        assert_eq!(code, 0);
        assert_eq!(std::fs::read_to_string(&file).unwrap(), "ok\n");
        assert_eq!(
            main_with(&args(&["mkdir", file.to_str().unwrap()]), &b""[..]),
            1
        );
        assert_eq!(main_with(&args(&["nada"]), &b""[..]), 2);
        std::fs::remove_dir_all(dir).unwrap();
    }
}
