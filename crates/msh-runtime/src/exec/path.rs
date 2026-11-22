//! Resolução do nome do comando (POSIX 2.9.1.4 e XBD 8.3, `PATH`).

use std::os::unix::fs::PermissionsExt;
use std::path::{Path, PathBuf};

use crate::shell::Shell;

/// `PATH` usado quando a variável não existe. O POSIX deixa a cargo da
/// implementação; este é o do dash (ADR-013).
pub const DEFAULT_PATH: &str = "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";

/// O nome não foi achado no `PATH`: `command not found`, status 127.
#[derive(Debug, Clone, PartialEq, Eq, thiserror::Error)]
#[error("{0}: command not found")]
pub struct NotFound(pub String);

/// Caminho a executar.
///
/// Nome com `/` é usado como está, e os erros (não existe, sem permissão,
/// diretório) aparecem no `execve`. Sem `/`, procura em cada diretório do
/// `PATH` um arquivo regular executável; entradas vazias valem como `.`. Um
/// arquivo sem permissão de execução não conta como achado (XBD 8.3), então
/// o resultado é 127, como no dash.
///
/// # Errors
///
/// [`NotFound`] se nenhum diretório do `PATH` tiver o comando, ou se o nome
/// for vazio.
pub fn resolve(name: &str, sh: &Shell) -> Result<PathBuf, NotFound> {
    if name.contains('/') {
        return Ok(PathBuf::from(name));
    }
    if name.is_empty() {
        return Err(NotFound(String::new()));
    }
    let path = sh.env.get("PATH").unwrap_or(DEFAULT_PATH);
    path.split(':')
        .map(|dir| {
            if dir.is_empty() {
                Path::new(".").join(name)
            } else {
                Path::new(dir).join(name)
            }
        })
        .find(|candidate| is_executable_file(candidate))
        .ok_or_else(|| NotFound(name.to_owned()))
}

fn is_executable_file(path: &Path) -> bool {
    let Ok(meta) = std::fs::metadata(path) else {
        return false;
    };
    meta.is_file()
        && meta.permissions().mode() & 0o111 != 0
        && nix::unistd::access(path, nix::unistd::AccessFlags::X_OK).is_ok()
}
