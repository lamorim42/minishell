//! Builtins. Cada um recebe os argumentos já expandidos (sem o nome do
//! comando) e devolve o status de saída.
//!
//! M2: `env`, `export` e `unset`. O M3 acrescenta `echo`, `cd`, `pwd` e
//! `exit`.

mod env;
mod export;
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
    Env,
    Export,
    Unset,
}

impl Builtin {
    #[must_use]
    pub fn lookup(name: &str) -> Option<Self> {
        match name {
            "env" => Some(Self::Env),
            "export" => Some(Self::Export),
            "unset" => Some(Self::Unset),
            _ => None,
        }
    }

    #[must_use]
    pub fn name(self) -> &'static str {
        match self {
            Self::Env => "env",
            Self::Export => "export",
            Self::Unset => "unset",
        }
    }

    /// Executa o builtin. Erro de escrita (por exemplo, pipe fechado) vira
    /// status 1, como no bash.
    pub fn run(self, args: &[String], io: &mut Io<'_>, sh: &mut Shell) -> i32 {
        let result = match self {
            Self::Env => env::run(args, io, sh),
            Self::Export => export::run(args, io, sh),
            Self::Unset => unset::run(args, io, sh),
        };
        result.unwrap_or_else(|e| {
            let _ = writeln!(io.err, "{}: {}: write error: {e}", sh.name, self.name());
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
