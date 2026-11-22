//! Copiar, mover, renomear, criar pasta e salvar arquivo (RF-23, RF-24).
//!
//! As mesmas funções servem à GUI (como o usuário) e ao helper de
//! administrador (como root, via polkit). Nenhuma delas sobrescreve um
//! arquivo existente sem querer: copiar escolhe um nome livre, e mover ou
//! renomear para um nome ocupado dá erro.

use std::fs;
use std::io::{self, Read, Write};
use std::os::unix::fs::{MetadataExt, OpenOptionsExt, PermissionsExt};
use std::path::{Path, PathBuf};

use crate::paths::{check_name, is_within, unique_name};

/// Tamanho máximo de um arquivo salvo pelo editor de administrador.
pub const MAX_WRITE: u64 = 16 * 1024 * 1024;

/// Copia `src` (arquivo, link ou pasta inteira) para dentro de `dest_dir`.
/// Se o nome já existir lá, usa `nome (cópia)`. Devolve o caminho criado.
///
/// # Errors
///
/// Erros de E/S, ou `InvalidInput` ao copiar uma pasta para dentro dela
/// mesma.
pub fn copy_into(src: &Path, dest_dir: &Path) -> io::Result<PathBuf> {
    let name = file_name(src)?;
    let meta = fs::symlink_metadata(src)?;
    if meta.is_dir() && is_within(dest_dir, src) {
        return Err(invalid(format!(
            "não dá para copiar “{name}” para dentro dela mesma"
        )));
    }
    let dest = dest_dir.join(unique_name(dest_dir, &name, |p| {
        fs::symlink_metadata(p).is_ok()
    }));
    copy_tree(src, &dest)?;
    Ok(dest)
}

/// Move `src` para dentro de `dest_dir`. Entre sistemas de arquivos
/// diferentes, copia e depois apaga o original.
///
/// # Errors
///
/// `AlreadyExists` se o nome já estiver ocupado em `dest_dir`;
/// `InvalidInput` ao mover uma pasta para dentro dela mesma; ou erros de E/S.
pub fn move_into(src: &Path, dest_dir: &Path) -> io::Result<PathBuf> {
    let name = file_name(src)?;
    let dest = dest_dir.join(&name);
    if dest == src {
        return Ok(dest);
    }
    let meta = fs::symlink_metadata(src)?;
    if meta.is_dir() && is_within(dest_dir, src) {
        return Err(invalid(format!(
            "não dá para mover “{name}” para dentro dela mesma"
        )));
    }
    rename_no_replace(src, &dest)?;
    Ok(dest)
}

/// Renomeia `path` para `new_name`, na mesma pasta.
///
/// # Errors
///
/// `InvalidInput` para nomes inválidos, `AlreadyExists` se o nome estiver
/// ocupado, ou erros de E/S.
pub fn rename(path: &Path, new_name: &str) -> io::Result<PathBuf> {
    check_name(new_name)?;
    let dest = crate::paths::parent_or_root(path).join(new_name);
    if dest == path {
        return Ok(dest);
    }
    rename_no_replace(path, &dest)?;
    Ok(dest)
}

/// Cria a pasta `path` (o pai precisa existir).
///
/// # Errors
///
/// Erros de E/S, inclusive `AlreadyExists`.
pub fn make_dir(path: &Path) -> io::Result<()> {
    fs::create_dir(path)
}

/// Grava `content` em `path` de forma atômica: escreve num arquivo novo na
/// mesma pasta e troca com `rename`. Se `path` já existir, ele precisa ser
/// um arquivo comum (não um link), e o novo herda o modo e o dono dele.
///
/// # Errors
///
/// `InvalidInput` se `path` for link ou pasta, ou erros de E/S.
pub fn write_file(path: &Path, content: &[u8]) -> io::Result<()> {
    let name = file_name(path)?;
    let dir = crate::paths::parent_or_root(path);
    let old = match fs::symlink_metadata(path) {
        Ok(m) if m.file_type().is_file() => Some(m),
        Ok(_) => return Err(invalid(format!("“{name}” não é um arquivo comum"))),
        Err(e) if e.kind() == io::ErrorKind::NotFound => None,
        Err(e) => return Err(e),
    };
    let tmp = dir.join(format!(".{name}.miau-{}", std::process::id()));
    let mut file = fs::OpenOptions::new()
        .write(true)
        .create_new(true)
        .mode(old.as_ref().map_or(0o644, |m| m.mode() & 0o7777))
        .custom_flags(libc_o_nofollow())
        .open(&tmp)?;
    let result = (|| {
        file.write_all(content)?;
        if let Some(m) = &old {
            // create_new aplica a umask; o modo antigo volta inteiro.
            file.set_permissions(fs::Permissions::from_mode(m.mode() & 0o7777))?;
            if nix::unistd::geteuid().is_root() {
                nix::unistd::fchown(
                    &file,
                    Some(nix::unistd::Uid::from_raw(m.uid())),
                    Some(nix::unistd::Gid::from_raw(m.gid())),
                )?;
            }
        }
        file.sync_all()?;
        fs::rename(&tmp, path)
    })();
    if result.is_err() {
        let _ = fs::remove_file(&tmp);
    }
    result
}

/// Lê no máximo [`MAX_WRITE`] bytes de `input` (o conteúdo que o helper
/// recebe pelo stdin).
///
/// # Errors
///
/// `InvalidData` se passar do limite, ou erros de leitura.
pub fn read_limited(input: impl Read) -> io::Result<Vec<u8>> {
    let mut buf = Vec::new();
    input.take(MAX_WRITE + 1).read_to_end(&mut buf)?;
    if buf.len() as u64 > MAX_WRITE {
        return Err(io::Error::new(
            io::ErrorKind::InvalidData,
            "arquivo grande demais (limite de 16 MiB)",
        ));
    }
    Ok(buf)
}

fn copy_tree(src: &Path, dest: &Path) -> io::Result<()> {
    let meta = fs::symlink_metadata(src)?;
    let kind = meta.file_type();
    if kind.is_symlink() {
        std::os::unix::fs::symlink(fs::read_link(src)?, dest)
    } else if kind.is_dir() {
        fs::create_dir(dest)?;
        for entry in fs::read_dir(src)? {
            let entry = entry?;
            copy_tree(&entry.path(), &dest.join(entry.file_name()))?;
        }
        fs::set_permissions(dest, fs::Permissions::from_mode(meta.mode() & 0o7777))
    } else if kind.is_file() {
        // fs::copy já copia as permissões.
        fs::copy(src, dest).map(drop)
    } else {
        Err(invalid(format!(
            "“{}” é um arquivo especial e não pode ser copiado",
            src.display()
        )))
    }
}

/// `rename` que não sobrescreve. Entre sistemas de arquivos (`EXDEV`),
/// copia e apaga o original.
fn rename_no_replace(src: &Path, dest: &Path) -> io::Result<()> {
    if fs::symlink_metadata(dest).is_ok() {
        return Err(io::Error::new(
            io::ErrorKind::AlreadyExists,
            format!("“{}” já existe", dest.display()),
        ));
    }
    match fs::rename(src, dest) {
        Err(e) if e.raw_os_error() == Some(nix::errno::Errno::EXDEV as i32) => {
            copy_tree(src, dest)?;
            if fs::symlink_metadata(src)?.is_dir() {
                fs::remove_dir_all(src)
            } else {
                fs::remove_file(src)
            }
        }
        other => other,
    }
}

fn libc_o_nofollow() -> i32 {
    nix::fcntl::OFlag::O_NOFOLLOW.bits()
}

fn file_name(path: &Path) -> io::Result<String> {
    path.file_name()
        .map(|n| n.to_string_lossy().into_owned())
        .ok_or_else(|| invalid(format!("“{}” não tem nome", path.display())))
}

fn invalid(msg: String) -> io::Error {
    io::Error::new(io::ErrorKind::InvalidInput, msg)
}

#[cfg(test)]
mod tests {
    use super::*;

    struct TempDir(PathBuf);
    impl TempDir {
        fn new(tag: &str) -> Self {
            let dir = std::env::temp_dir().join(format!(
                "msh-ops-{tag}-{}-{:?}",
                std::process::id(),
                std::thread::current().id()
            ));
            let _ = fs::remove_dir_all(&dir);
            fs::create_dir_all(&dir).unwrap();
            Self(dir)
        }
    }
    impl Drop for TempDir {
        fn drop(&mut self) {
            let _ = fs::remove_dir_all(&self.0);
        }
    }

    #[test]
    fn copy_file_and_tree_with_unique_names() {
        let t = TempDir::new("copy");
        let src = t.0.join("src");
        fs::create_dir_all(src.join("sub")).unwrap();
        fs::write(src.join("a.txt"), "a").unwrap();
        fs::write(src.join("sub/b.sh"), "b").unwrap();
        fs::set_permissions(src.join("sub/b.sh"), fs::Permissions::from_mode(0o755)).unwrap();
        std::os::unix::fs::symlink("a.txt", src.join("link")).unwrap();
        let dest = t.0.join("dest");
        fs::create_dir(&dest).unwrap();

        let copied = copy_into(&src, &dest).unwrap();
        assert_eq!(copied, dest.join("src"));
        assert_eq!(fs::read_to_string(copied.join("sub/b.sh")).unwrap(), "b");
        assert_eq!(
            fs::metadata(copied.join("sub/b.sh")).unwrap().mode() & 0o777,
            0o755
        );
        assert_eq!(
            fs::read_link(copied.join("link")).unwrap(),
            Path::new("a.txt")
        );

        // De novo: não sobrescreve.
        assert_eq!(copy_into(&src, &dest).unwrap(), dest.join("src (cópia)"));
        let file = copy_into(&src.join("a.txt"), &src).unwrap();
        assert_eq!(file, src.join("a (cópia).txt"));
    }

    #[test]
    fn refuses_copy_or_move_into_itself() {
        let t = TempDir::new("self");
        let dir = t.0.join("d");
        fs::create_dir_all(dir.join("in")).unwrap();
        let e = copy_into(&dir, &dir.join("in")).unwrap_err();
        assert_eq!(e.kind(), io::ErrorKind::InvalidInput);
        let e = move_into(&dir, &dir.join("in")).unwrap_err();
        assert_eq!(e.kind(), io::ErrorKind::InvalidInput);
    }

    #[test]
    fn move_and_rename_never_overwrite() {
        let t = TempDir::new("move");
        fs::write(t.0.join("a"), "1").unwrap();
        fs::create_dir(t.0.join("d")).unwrap();
        fs::write(t.0.join("d/a"), "2").unwrap();
        let e = move_into(&t.0.join("a"), &t.0.join("d")).unwrap_err();
        assert_eq!(e.kind(), io::ErrorKind::AlreadyExists);
        assert_eq!(fs::read_to_string(t.0.join("d/a")).unwrap(), "2");

        let renamed = rename(&t.0.join("a"), "b").unwrap();
        assert_eq!(renamed, t.0.join("b"));
        let moved = move_into(&renamed, &t.0.join("d")).unwrap();
        assert_eq!(fs::read_to_string(moved).unwrap(), "1");
        assert!(!t.0.join("b").exists());

        // Mover para a pasta onde já está não faz nada.
        assert_eq!(
            move_into(&t.0.join("d/a"), &t.0.join("d")).unwrap(),
            t.0.join("d/a")
        );
        assert_eq!(
            rename(&t.0.join("d/a"), "b").unwrap_err().kind(),
            io::ErrorKind::AlreadyExists
        );
        assert_eq!(
            rename(&t.0.join("d/a"), "x/y").unwrap_err().kind(),
            io::ErrorKind::InvalidInput
        );
    }

    #[test]
    fn write_keeps_mode_and_refuses_links() {
        let t = TempDir::new("write");
        let f = t.0.join("hosts");
        fs::write(&f, "velho").unwrap();
        fs::set_permissions(&f, fs::Permissions::from_mode(0o600)).unwrap();
        write_file(&f, b"novo").unwrap();
        assert_eq!(fs::read_to_string(&f).unwrap(), "novo");
        assert_eq!(fs::metadata(&f).unwrap().mode() & 0o7777, 0o600);

        write_file(&t.0.join("novo.conf"), b"x").unwrap();
        assert_eq!(fs::read_to_string(t.0.join("novo.conf")).unwrap(), "x");

        std::os::unix::fs::symlink(&f, t.0.join("link")).unwrap();
        let e = write_file(&t.0.join("link"), b"y").unwrap_err();
        assert_eq!(e.kind(), io::ErrorKind::InvalidInput);
        assert_eq!(fs::read_to_string(&f).unwrap(), "novo");
        // Nada de temporário sobrando.
        assert_eq!(fs::read_dir(&t.0).unwrap().count(), 3);
    }

    #[test]
    fn limited_read() {
        assert_eq!(read_limited(&b"abc"[..]).unwrap(), b"abc");
        let big = vec![0_u8; usize::try_from(MAX_WRITE).unwrap() + 1];
        assert_eq!(
            read_limited(&big[..]).unwrap_err().kind(),
            io::ErrorKind::InvalidData
        );
    }
}
