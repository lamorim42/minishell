//! `pwd [-L|-P]` (POSIX). Com `-L` (o padrão), mostra o `PWD` se ele for um
//! caminho absoluto válido para o diretório atual; senão, e com `-P`, mostra
//! o caminho físico.

use std::io;

use super::{Io, invalid_option};
use crate::shell::{Shell, pwd_is_valid};

pub(super) fn run(args: &[String], io: &mut Io<'_>, sh: &Shell) -> io::Result<i32> {
    let mut physical = false;
    for arg in args {
        match arg.as_str() {
            "-L" => physical = false,
            "-P" => physical = true,
            "--" => break,
            opt if opt.starts_with('-') && opt.len() > 1 => {
                return invalid_option(io, sh, "pwd", opt);
            }
            // Operandos são ignorados, como no dash e no bash.
            _ => break,
        }
    }
    if !physical && let Some(pwd) = sh.env.get("PWD").filter(|p| pwd_is_valid(p)) {
        writeln!(io.out, "{pwd}")?;
        return Ok(0);
    }
    match std::env::current_dir() {
        Ok(cwd) => {
            writeln!(io.out, "{}", cwd.display())?;
            Ok(0)
        }
        Err(e) => {
            writeln!(io.err, "{}: pwd: {}", sh.name, super::strerror(&e))?;
            Ok(1)
        }
    }
}
