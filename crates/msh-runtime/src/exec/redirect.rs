//! Redirecionamentos (POSIX 2.7): `<`, `>`, `>>` e `<<`, aplicados em
//! ordem, da esquerda para a direita.
//!
//! No filho, os fds 0 e 1 são trocados de vez. Num builtin que roda no pai,
//! [`Saved`] guarda cópias dos fds originais e os devolve no `Drop`.

use std::fs::OpenOptions;
use std::os::fd::{AsFd, OwnedFd};
use std::os::unix::fs::OpenOptionsExt;

use msh_syntax::Redirect;

use super::sys;
use crate::builtins::strerror;
use crate::expand::expand_redirect_target;
use crate::shell::Shell;

/// Modo de criação de arquivos: `0666` menos a `umask`, como no POSIX e no
/// bash (o TDD dizia 0644, que é o resultado com a umask comum 022).
const CREATE_MODE: u32 = 0o666;

/// Erro ao aplicar um redirecionamento: a mensagem já está pronta, e o
/// comando falha com status 1 (POSIX 2.8.1).
#[derive(Debug, Clone, PartialEq, Eq, thiserror::Error)]
#[error("{0}")]
pub struct RedirectError(pub String);

/// Fds padrão guardados antes de um redirecionamento no pai.
#[derive(Debug, Default)]
pub struct Saved {
    fds: Vec<(i32, OwnedFd)>,
}

impl Saved {
    fn save(&mut self, target: i32) -> Result<(), RedirectError> {
        if self.fds.iter().any(|(t, _)| *t == target) {
            return Ok(());
        }
        let fd = match target {
            0 => std::io::stdin().as_fd().try_clone_to_owned(),
            1 => std::io::stdout().as_fd().try_clone_to_owned(),
            _ => std::io::stderr().as_fd().try_clone_to_owned(),
        };
        // `try_clone_to_owned` usa F_DUPFD_CLOEXEC, então a cópia não vaza
        // para os filhos.
        let fd = fd.map_err(|e| RedirectError(format!("dup: {}", strerror(&e))))?;
        self.fds.push((target, fd));
        Ok(())
    }
}

impl Drop for Saved {
    fn drop(&mut self) {
        use std::io::Write;
        let _ = std::io::stdout().flush();
        for (target, fd) in self.fds.drain(..).rev() {
            let _ = sys::dup2_std(fd.as_fd(), target);
        }
    }
}

/// Aplica `redirects`. `heredocs` tem os corpos já lidos, na ordem dos `<<`.
/// Com `saved`, guarda os fds originais para o `Drop` devolver.
///
/// # Errors
///
/// Arquivo que não abre: [`RedirectError`] com a mensagem.
/// Os redirecionamentos anteriores ao que falhou continuam aplicados (e são
/// desfeitos pelo `Saved`).
pub fn apply(
    redirects: &[Redirect],
    heredocs: &mut impl Iterator<Item = OwnedFd>,
    sh: &Shell,
    mut saved: Option<&mut Saved>,
) -> Result<(), RedirectError> {
    for r in redirects {
        let (fd, target) = match r {
            Redirect::In(word) => {
                let path = expand_redirect_target(word, sh);
                (open(&path, OpenOptions::new().read(true))?, 0)
            }
            Redirect::Out(word) => {
                let path = expand_redirect_target(word, sh);
                let mut opts = OpenOptions::new();
                opts.write(true)
                    .create(true)
                    .truncate(true)
                    .mode(CREATE_MODE);
                (open(&path, &opts)?, 1)
            }
            Redirect::Append(word) => {
                let path = expand_redirect_target(word, sh);
                let mut opts = OpenOptions::new();
                opts.append(true).create(true).mode(CREATE_MODE);
                (open(&path, &opts)?, 1)
            }
            Redirect::HereDoc { .. } => {
                let fd = heredocs
                    .next()
                    .ok_or_else(|| RedirectError("heredoc: body not read".into()))?;
                (fd, 0)
            }
        };
        if let Some(saved) = saved.as_deref_mut() {
            if target == 1 {
                use std::io::Write;
                let _ = std::io::stdout().flush();
            }
            saved.save(target)?;
        }
        sys::dup2_std(fd.as_fd(), target)
            .map_err(|e| RedirectError(format!("dup2: {}", e.desc())))?;
        // `fd` (com O_CLOEXEC) fecha aqui; o 0 ou o 1 continua aberto.
    }
    Ok(())
}

fn open(path: &str, opts: &OpenOptions) -> Result<OwnedFd, RedirectError> {
    // O std abre com O_CLOEXEC.
    opts.open(path)
        .map(OwnedFd::from)
        .map_err(|e| RedirectError(format!("{path}: {}", strerror(&e))))
}
