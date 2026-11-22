//! Heredoc (`<<`, POSIX 2.7.4): lido no pai, antes de qualquer `fork`, e
//! guardado num `memfd_create` (ADR-009), sem arquivo temporário e sem o
//! limite de 64 KiB de um pipe.

use std::io::Write;
use std::os::fd::OwnedFd;

use msh_syntax::{Command, Pipeline, Redirect};
use nix::sys::memfd::{MFdFlags, memfd_create};
use nix::unistd::{Whence, lseek};

use crate::input::{LineSource, ReadLine};
use crate::shell::Shell;

/// Prompt das linhas do heredoc (o `PS2` do POSIX).
pub const PROMPT: &str = "> ";

/// Ctrl+C durante a leitura: a linha inteira é cancelada.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Cancelled;

/// Lê todos os heredocs da pipeline, na ordem em que aparecem. O resultado
/// tem um `Vec` por comando, na mesma ordem dos `<<` daquele comando.
///
/// # Errors
///
/// [`Cancelled`] se o usuário apertar Ctrl+C.
pub fn read_all(
    pipeline: &Pipeline,
    sh: &Shell,
    input: &mut dyn LineSource,
) -> Result<Vec<Vec<OwnedFd>>, Cancelled> {
    let mut all = Vec::with_capacity(pipeline.commands.len());
    for cmd in &pipeline.commands {
        let redirects = match cmd {
            Command::Simple(c) => &c.redirects,
            Command::Subshell { redirects, .. } => redirects,
        };
        let mut fds = Vec::new();
        for r in redirects {
            if let Redirect::HereDoc { delimiter, expand } = r {
                let body = read_body(delimiter, input, sh)?;
                let body = if *expand {
                    expand_body(&body, sh)
                } else {
                    body
                };
                match to_memfd(&body) {
                    Ok(fd) => fds.push(fd),
                    Err(e) => {
                        sh.error(format_args!("heredoc: {}", e.desc()));
                        return Err(Cancelled);
                    }
                }
            }
        }
        all.push(fds);
    }
    Ok(all)
}

/// Linhas até uma linha igual ao delimitador. No fim da entrada, avisa como
/// o bash e usa o que foi lido.
fn read_body(delimiter: &str, input: &mut dyn LineSource, sh: &Shell) -> Result<String, Cancelled> {
    let mut body = String::new();
    loop {
        match input.read_line(PROMPT) {
            ReadLine::Text(line) if line == delimiter => return Ok(body),
            ReadLine::Text(line) => {
                body.push_str(&line);
                body.push('\n');
            }
            ReadLine::Interrupted => return Err(Cancelled),
            ReadLine::Eof => {
                sh.error(format_args!(
                    "warning: here-document delimited by end-of-file (wanted `{delimiter}')"
                ));
                return Ok(body);
            }
        }
    }
}

fn to_memfd(body: &str) -> nix::Result<OwnedFd> {
    let fd = memfd_create("minishell-heredoc", MFdFlags::MFD_CLOEXEC)?;
    let mut file = std::fs::File::from(fd);
    file.write_all(body.as_bytes())
        .map_err(|e| nix::errno::Errno::from_raw(e.raw_os_error().unwrap_or(0)))?;
    let fd = OwnedFd::from(file);
    lseek(&fd, 0, Whence::SeekSet)?;
    Ok(fd)
}

/// Expansão do corpo quando o delimitador não tinha aspas (POSIX 2.7.4):
/// parâmetros (`$A`, `${A}`, `$?`...), e `\` só é especial antes de `$`,
/// `` ` ``, `\` e newline, como entre aspas duplas (mas `"` não é especial).
#[must_use]
pub fn expand_body(body: &str, sh: &Shell) -> String {
    let chars: Vec<char> = body.chars().collect();
    let mut out = String::with_capacity(body.len());
    let mut i = 0;
    while i < chars.len() {
        let c = chars[i];
        match c {
            '\\' => match chars.get(i + 1) {
                Some(&n @ ('$' | '`' | '\\')) => {
                    out.push(n);
                    i += 2;
                }
                Some('\n') => i += 2,
                _ => {
                    out.push('\\');
                    i += 1;
                }
            },
            '$' => {
                let (value, used) = parameter(&chars[i + 1..], sh);
                if let Some(v) = value {
                    out.push_str(&v);
                    i += 1 + used;
                } else {
                    out.push('$');
                    i += 1;
                }
            }
            _ => {
                out.push(c);
                i += 1;
            }
        }
    }
    out
}

/// Parâmetro logo depois de um `$`. Devolve o valor (vazio se não existe) e
/// quantos caracteres foram usados; `None` quando o `$` é literal.
fn parameter(rest: &[char], sh: &Shell) -> (Option<String>, usize) {
    let value = |name: &str| sh.param(name).unwrap_or_default();
    match rest.first() {
        Some('{') => {
            let Some(end) = rest.iter().position(|&c| c == '}') else {
                return (None, 0);
            };
            let name: String = rest[1..end].iter().collect();
            if crate::env::is_valid_name(&name)
                || (name.len() == 1 && "?$#0@*!-".contains(&name))
                || (!name.is_empty() && name.chars().all(|c| c.is_ascii_digit()))
            {
                (Some(value(&name)), end + 1)
            } else {
                (None, 0)
            }
        }
        Some(&c) if c.is_ascii_alphabetic() || c == '_' => {
            let len = rest
                .iter()
                .take_while(|c| c.is_ascii_alphanumeric() || **c == '_')
                .count();
            let name: String = rest[..len].iter().collect();
            (Some(value(&name)), len)
        }
        Some(&c) if c.is_ascii_digit() || "?$#@*!-".contains(c) => (Some(value(&c.to_string())), 1),
        _ => (None, 0),
    }
}

#[cfg(test)]
mod tests {
    use super::expand_body;
    use crate::{Env, Shell};

    #[test]
    fn expands_like_double_quotes() {
        let mut sh = Shell::new(Env::from_pairs([("A", "1"), ("HOME", "/h")]));
        sh.last_status = 7;
        assert_eq!(expand_body("$A ${A}b $Ab $?\n", &sh), "1 1b  7\n");
        assert_eq!(
            expand_body("\\$A \\\\ \\\" '$A' \"$A\"", &sh),
            "$A \\ \\\" '1' \"1\""
        );
        assert_eq!(expand_body("custa $ 5 e ${", &sh), "custa $ 5 e ${");
        assert_eq!(expand_body("a\\\nb", &sh), "ab");
        assert_eq!(expand_body("~ fica", &sh), "~ fica");
    }
}
