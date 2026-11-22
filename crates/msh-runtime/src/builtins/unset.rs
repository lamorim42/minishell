//! `unset [-v] NOME...` (POSIX, special built-in `unset`). Não há funções
//! no minishell, então `-f` não é aceito.

use std::io;

use super::{Io, invalid_identifier, invalid_option};
use crate::env::is_valid_name;
use crate::shell::Shell;

pub(super) fn run(args: &[String], io: &mut Io<'_>, sh: &mut Shell) -> io::Result<i32> {
    let mut args = args;
    if let Some(first) = args.first() {
        match first.as_str() {
            "-v" | "--" => args = &args[1..],
            opt if opt.starts_with('-') && opt.len() > 1 => {
                return invalid_option(io, sh, "unset", opt);
            }
            _ => {}
        }
    }

    let mut status = 0;
    for name in args {
        if is_valid_name(name) {
            sh.env.unset(name);
        } else {
            invalid_identifier(io, sh, "unset", name)?;
            status = 1;
        }
    }
    Ok(status)
}
