//! Favoritos da barra lateral (RF-23), salvos em
//! `$XDG_CONFIG_HOME/miau-files/bookmarks` (ou `~/.config/...`).
//!
//! O formato é o mesmo do `~/.config/gtk-3.0/bookmarks`: uma URI `file://`
//! por linha, seguida opcionalmente de um espaço e do rótulo.

use std::io;
use std::path::{Path, PathBuf};

use crate::uri::{file_uri_path, path_file_uri};

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Bookmark {
    pub path: PathBuf,
    /// Rótulo escolhido pelo usuário; sem ele, a GUI mostra o nome da pasta.
    pub label: Option<String>,
}

impl Bookmark {
    #[must_use]
    pub fn new(path: PathBuf) -> Self {
        Self { path, label: None }
    }

    /// O texto que aparece na barra lateral.
    #[must_use]
    pub fn title(&self) -> String {
        self.label.clone().unwrap_or_else(|| {
            self.path.file_name().map_or_else(
                || self.path.to_string_lossy().into_owned(),
                |n| n.to_string_lossy().into_owned(),
            )
        })
    }
}

/// Onde os favoritos ficam (diretórios XDG).
#[must_use]
pub fn default_file(xdg_config_home: Option<&Path>, home: Option<&Path>) -> Option<PathBuf> {
    let base = match xdg_config_home {
        Some(dir) if dir.is_absolute() => dir.to_path_buf(),
        _ => home?.join(".config"),
    };
    Some(base.join("miau-files").join("bookmarks"))
}

/// Lê os favoritos do texto do arquivo. Linhas inválidas são ignoradas.
#[must_use]
pub fn parse(text: &str) -> Vec<Bookmark> {
    let mut out: Vec<Bookmark> = Vec::new();
    for line in text.lines() {
        let (uri, label) = match line.split_once(' ') {
            Some((uri, label)) if !label.trim().is_empty() => (uri, Some(label.trim())),
            _ => (line.trim(), None),
        };
        if let Some(path) = file_uri_path(uri)
            && !out.iter().any(|b| b.path == path)
        {
            out.push(Bookmark {
                path,
                label: label.map(str::to_owned),
            });
        }
    }
    out
}

/// O texto do arquivo, no mesmo formato que [`parse`] lê.
#[must_use]
pub fn serialize(bookmarks: &[Bookmark]) -> String {
    let mut out = String::new();
    for b in bookmarks {
        out.push_str(&path_file_uri(&b.path));
        if let Some(label) = &b.label {
            out.push(' ');
            out.push_str(label);
        }
        out.push('\n');
    }
    out
}

/// Acrescenta `path`, se ainda não estiver lá. Devolve `true` se mudou.
pub fn add(bookmarks: &mut Vec<Bookmark>, path: PathBuf) -> bool {
    if bookmarks.iter().any(|b| b.path == path) {
        return false;
    }
    bookmarks.push(Bookmark::new(path));
    true
}

/// Remove `path`. Devolve `true` se estava lá.
pub fn remove(bookmarks: &mut Vec<Bookmark>, path: &Path) -> bool {
    let before = bookmarks.len();
    bookmarks.retain(|b| b.path != path);
    bookmarks.len() != before
}

/// Lê o arquivo; se ele não existir, a lista começa vazia.
///
/// # Errors
///
/// Erros de leitura que não sejam "arquivo não existe".
pub fn load(file: &Path) -> io::Result<Vec<Bookmark>> {
    match std::fs::read_to_string(file) {
        Ok(text) => Ok(parse(&text)),
        Err(e) if e.kind() == io::ErrorKind::NotFound => Ok(Vec::new()),
        Err(e) => Err(e),
    }
}

/// Grava o arquivo, criando a pasta se preciso. A escrita é atômica (arquivo
/// temporário + `rename`), para não perder os favoritos se a GUI cair.
///
/// # Errors
///
/// Erros de criação da pasta ou de escrita.
pub fn save(file: &Path, bookmarks: &[Bookmark]) -> io::Result<()> {
    if let Some(dir) = file.parent() {
        std::fs::create_dir_all(dir)?;
    }
    let tmp = file.with_extension("tmp");
    std::fs::write(&tmp, serialize(bookmarks))?;
    std::fs::rename(&tmp, file)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn xdg_location() {
        assert_eq!(
            default_file(Some(Path::new("/x")), Some(Path::new("/home/ana"))),
            Some(PathBuf::from("/x/miau-files/bookmarks"))
        );
        // XDG relativo é inválido pela especificação e é ignorado.
        assert_eq!(
            default_file(Some(Path::new("x")), Some(Path::new("/home/ana"))),
            Some(PathBuf::from("/home/ana/.config/miau-files/bookmarks"))
        );
        assert_eq!(default_file(None, None), None);
    }

    #[test]
    fn parse_and_serialize() {
        let text = "file:///home/ana/Projetos\nfile:///srv/meus%20dados Dados\n\nlixo\nfile:///home/ana/Projetos\n";
        let list = parse(text);
        assert_eq!(
            list,
            vec![
                Bookmark::new("/home/ana/Projetos".into()),
                Bookmark {
                    path: "/srv/meus dados".into(),
                    label: Some("Dados".into())
                },
            ]
        );
        assert_eq!(parse(&serialize(&list)), list);
        assert_eq!(list[0].title(), "Projetos");
        assert_eq!(list[1].title(), "Dados");
        assert_eq!(Bookmark::new("/".into()).title(), "/");
    }

    #[test]
    fn add_remove_and_files() {
        let dir = std::env::temp_dir().join(format!("msh-bm-{}", std::process::id()));
        let file = dir.join("miau-files/bookmarks");
        assert_eq!(load(&file).unwrap(), vec![]);

        let mut list = Vec::new();
        assert!(add(&mut list, "/etc".into()));
        assert!(!add(&mut list, "/etc".into()));
        assert!(add(&mut list, "/tmp".into()));
        save(&file, &list).unwrap();
        assert_eq!(load(&file).unwrap(), list);

        assert!(remove(&mut list, Path::new("/etc")));
        assert!(!remove(&mut list, Path::new("/etc")));
        assert_eq!(list, vec![Bookmark::new("/tmp".into())]);
        std::fs::remove_dir_all(dir).unwrap();
    }
}
