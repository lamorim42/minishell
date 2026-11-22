//! `cd [-L|-P] [diretório]` e `cd -` (POSIX, utilitário `cd`).
//!
//! Segue os passos da especificação: `HOME` sem operando, `OLDPWD` com `-`
//! (imprimindo o novo diretório), busca em `CDPATH` e, no modo lógico
//! (`-L`, o padrão), o caminho é montado a partir do `PWD` e canonizado sem
//! seguir links. Atualiza `PWD` e `OLDPWD`, que ficam exportadas.

use std::io;
use std::path::Path;

use super::{Io, invalid_option, strerror};
use crate::shell::{Shell, pwd_is_valid};

pub(super) fn run(args: &[String], io: &mut Io<'_>, sh: &mut Shell) -> io::Result<i32> {
    let mut physical = false;
    let mut rest = args;
    while let Some(first) = rest.first() {
        match first.as_str() {
            "-L" => physical = false,
            "-P" => physical = true,
            "--" => {
                rest = &rest[1..];
                break;
            }
            opt if opt.starts_with('-') && opt.len() > 1 => {
                return invalid_option(io, sh, "cd", opt);
            }
            _ => break,
        }
        rest = &rest[1..];
    }

    let (dir, mut print) = match rest {
        [] => match sh.env.get("HOME").filter(|h| !h.is_empty()) {
            Some(home) => (home.to_owned(), false),
            None => return fail(io, sh, "HOME not set"),
        },
        [dash] if dash == "-" => match sh.env.get("OLDPWD").filter(|h| !h.is_empty()) {
            Some(old) => (old.to_owned(), true),
            None => return fail(io, sh, "OLDPWD not set"),
        },
        [dir] => (dir.clone(), false),
        [_, _, ..] => return fail(io, sh, "too many arguments"),
    };
    if dir.is_empty() {
        // POSIX: operando vazio não muda de diretório (resultado
        // não especificado; o dash e o bash não fazem nada).
        return Ok(0);
    }

    let mut curpath = dir.clone();
    if !dir.starts_with('/')
        && !starts_with_dot_component(&dir)
        && let Some(found) = search_cdpath(&dir, sh)
    {
        // Diretório achado por uma entrada não vazia do CDPATH: imprime.
        print |= found.1;
        curpath = found.0;
    }

    let old = current_pwd(sh);
    let new = if physical {
        curpath.clone()
    } else {
        let absolute = if curpath.starts_with('/') {
            curpath.clone()
        } else {
            format!("{}/{curpath}", old.as_deref().unwrap_or("."))
        };
        canonicalize_lexically(&absolute)
    };

    if let Err(e) = std::env::set_current_dir(&new) {
        writeln!(io.err, "{}: cd: {dir}: {}", sh.name, strerror(&e))?;
        return Ok(1);
    }

    let pwd = if physical {
        std::env::current_dir().map_or(new, |p| p.to_string_lossy().into_owned())
    } else {
        new
    };
    if let Some(old) = old {
        sh.env.export("OLDPWD", Some(old));
    }
    if print {
        writeln!(io.out, "{pwd}")?;
    }
    sh.env.export("PWD", Some(pwd));
    Ok(0)
}

fn fail(io: &mut Io<'_>, sh: &Shell, msg: &str) -> io::Result<i32> {
    writeln!(io.err, "{}: cd: {msg}", sh.name)?;
    Ok(1)
}

/// `PWD` atual, se for válido; senão o caminho físico.
fn current_pwd(sh: &Shell) -> Option<String> {
    sh.env
        .get("PWD")
        .filter(|p| pwd_is_valid(p))
        .map(str::to_owned)
        .or_else(|| {
            std::env::current_dir()
                .ok()
                .map(|p| p.to_string_lossy().into_owned())
        })
}

fn starts_with_dot_component(dir: &str) -> bool {
    let first = dir.split('/').next().unwrap_or_default();
    first == "." || first == ".."
}

/// Procura `dir` em cada entrada do `CDPATH`. Retorna o caminho e se ele deve
/// ser impresso (entrada não vazia).
fn search_cdpath(dir: &str, sh: &Shell) -> Option<(String, bool)> {
    let cdpath = sh.env.get("CDPATH")?;
    for entry in cdpath.split(':') {
        let candidate = if entry.is_empty() {
            format!("./{dir}")
        } else if entry.ends_with('/') {
            format!("{entry}{dir}")
        } else {
            format!("{entry}/{dir}")
        };
        if Path::new(&candidate).is_dir() {
            return Some((candidate, !entry.is_empty()));
        }
    }
    None
}

/// Remove `.`, `..` e barras repetidas de um caminho absoluto, sem consultar
/// o sistema de arquivos (passo 8 do `cd` no POSIX).
#[must_use]
pub(crate) fn canonicalize_lexically(path: &str) -> String {
    let mut parts: Vec<&str> = Vec::new();
    for comp in path.split('/') {
        match comp {
            "" | "." => {}
            ".." => {
                parts.pop();
            }
            c => parts.push(c),
        }
    }
    format!("/{}", parts.join("/"))
}

#[cfg(test)]
mod tests {
    use super::canonicalize_lexically as canon;

    #[test]
    fn lexical_canonicalization() {
        assert_eq!(canon("/"), "/");
        assert_eq!(canon("/a/b/../c"), "/a/c");
        assert_eq!(canon("/a/./b//"), "/a/b");
        assert_eq!(canon("/.."), "/");
        assert_eq!(canon("/a/b/../../.."), "/");
    }
}
