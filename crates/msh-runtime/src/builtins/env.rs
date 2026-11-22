//! `env` sem opções nem argumentos: lista as variáveis exportadas com valor.

use std::io;

use super::Io;
use crate::shell::Shell;

/// Status do `env` quando ele mesmo falha (como o `env` do coreutils).
const ENV_FAILURE: i32 = 125;

pub(super) fn run(args: &[String], io: &mut Io<'_>, sh: &Shell) -> io::Result<i32> {
    if let Some(arg) = args.first() {
        writeln!(
            io.err,
            "{}: env: `{arg}': options and arguments are not supported",
            sh.name
        )?;
        return Ok(ENV_FAILURE);
    }
    for (name, var) in sh.env.exported() {
        if let Some(value) = &var.value {
            writeln!(io.out, "{name}={value}")?;
        }
    }
    Ok(0)
}
