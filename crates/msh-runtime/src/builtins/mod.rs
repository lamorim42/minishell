//! Builtins. Cada um recebe os argumentos já expandidos (sem o nome do
//! comando) e devolve o status de saída.
//!
//! M2: `env`, `export` e `unset`. M3: `echo`, `cd`, `pwd` e `exit`.

mod cd;
mod echo;
mod env;
mod exit;
mod export;
mod pwd;
mod unset;

use std::io::Write;

use crate::shell::Shell;

/// Saídas do builtin. Nos testes são buffers em memória; na execução, os fds
/// 1 e 2 já redirecionados.
pub struct Io<'a> {
    pub out: &'a mut dyn Write,
    pub err: &'a mut dyn Write,
}

impl std::fmt::Debug for Io<'_> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("Io").finish_non_exhaustive()
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Builtin {
    Cd,
    Echo,
    Env,
    Exit,
    Export,
    Pwd,
    Unset,
}

impl Builtin {
    #[must_use]
    pub fn lookup(name: &str) -> Option<Self> {
        match name {
            "cd" => Some(Self::Cd),
            "echo" => Some(Self::Echo),
            "env" => Some(Self::Env),
            "exit" => Some(Self::Exit),
            "export" => Some(Self::Export),
            "pwd" => Some(Self::Pwd),
            "unset" => Some(Self::Unset),
            _ => None,
        }
    }

    /// *Special built-in* do POSIX (2.15): atribuições antes dele ficam no
    /// shell, e um erro nele encerra o shell não interativo (2.8.1).
    #[must_use]
    pub fn is_special(self) -> bool {
        matches!(self, Self::Exit | Self::Export | Self::Unset)
    }

    #[must_use]
    pub fn name(self) -> &'static str {
        match self {
            Self::Cd => "cd",
            Self::Echo => "echo",
            Self::Env => "env",
            Self::Exit => "exit",
            Self::Export => "export",
            Self::Pwd => "pwd",
            Self::Unset => "unset",
        }
    }

    /// Executa o builtin. Erro de escrita (por exemplo, pipe fechado) vira
    /// status 1, como no bash.
    pub fn run(self, args: &[String], io: &mut Io<'_>, sh: &mut Shell) -> i32 {
        let result = match self {
            Self::Cd => cd::run(args, io, sh),
            Self::Echo => echo::run(args, io),
            Self::Env => env::run(args, io, sh),
            Self::Exit => exit::run(args, io, sh),
            Self::Export => export::run(args, io, sh),
            Self::Pwd => pwd::run(args, io, sh),
            Self::Unset => unset::run(args, io, sh),
        };
        let result = result.and_then(|status| io.out.flush().map(|()| status));
        result.unwrap_or_else(|e| {
            let _ = writeln!(
                io.err,
                "{}: {}: write error: {}",
                sh.name,
                self.name(),
                strerror(&e)
            );
            1
        })
    }
}

/// Mensagem do tipo ``minishell: export: `1a': not a valid identifier``.
fn invalid_identifier(
    io: &mut Io<'_>,
    sh: &Shell,
    builtin: &str,
    arg: &str,
) -> std::io::Result<()> {
    writeln!(
        io.err,
        "{}: {builtin}: `{arg}': not a valid identifier",
        sh.name
    )
}

/// `minishell: unset: -x: invalid option`, status 2.
fn invalid_option(io: &mut Io<'_>, sh: &Shell, builtin: &str, arg: &str) -> std::io::Result<i32> {
    writeln!(io.err, "{}: {builtin}: {arg}: invalid option", sh.name)?;
    Ok(2)
}

/// Texto do erro como o `strerror` do C, sem o "(os error N)" do Rust.
pub(crate) fn strerror(e: &std::io::Error) -> String {
    match e.raw_os_error() {
        Some(code) => nix::errno::Errno::from_raw(code).desc().to_owned(),
        None => e.to_string(),
    }
}
