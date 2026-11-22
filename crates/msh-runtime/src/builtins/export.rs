//! `export [-p] [NOME[=valor]]...` (POSIX, special built-in `export`).
//!
//! Sem argumentos (ou com `-p`), lista no formato do `bash --posix`:
//! `export NOME="valor"`, que pode ser colado de volta no shell.

use std::io;

use super::{Io, invalid_identifier, invalid_option};
use crate::env::is_valid_name;
use crate::shell::Shell;

pub(super) fn run(args: &[String], io: &mut Io<'_>, sh: &mut Shell) -> io::Result<i32> {
    let mut args = args;
    if let Some(first) = args.first() {
        match first.as_str() {
            "-p" | "--" => args = &args[1..],
            opt if opt.starts_with('-') && opt.len() > 1 => {
                return invalid_option(io, sh, "export", opt);
            }
            _ => {}
        }
    }
    if args.is_empty() {
        print_all(io, sh)?;
        return Ok(0);
    }

    let mut status = 0;
    for arg in args {
        let (name, value) = match arg.split_once('=') {
            Some((n, v)) => (n, Some(v.to_owned())),
            None => (arg.as_str(), None),
        };
        if is_valid_name(name) {
            sh.env.export(name, value);
        } else {
            invalid_identifier(io, sh, "export", arg)?;
            status = 1;
        }
    }
    Ok(status)
}

fn print_all(io: &mut Io<'_>, sh: &Shell) -> io::Result<()> {
    for (name, var) in sh.env.exported() {
        match &var.value {
            Some(value) => writeln!(io.out, "export {name}=\"{}\"", escape(value))?,
            None => writeln!(io.out, "export {name}")?,
        }
    }
    Ok(())
}

/// Escapa o que é especial dentro de aspas duplas (POSIX 2.2.3).
fn escape(value: &str) -> String {
    let mut out = String::with_capacity(value.len());
    for c in value.chars() {
        if matches!(c, '"' | '\\' | '$' | '`') {
            out.push('\\');
        }
        out.push(c);
    }
    out
}
