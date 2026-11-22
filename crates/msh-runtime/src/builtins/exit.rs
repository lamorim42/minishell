//! `exit [n]` (POSIX, special built-in `exit`).
//!
//! Sem argumento, sai com o status do último comando (POSIX; a versão em C
//! saía com 0). O código é tomado módulo 256; negativos são recusados. Argumento não numérico dá
//! `numeric argument required` e sai com 2; mais de um argumento dá
//! `too many arguments`, status 1, e o shell interativo continua.

use std::io;

use super::Io;
use crate::shell::Shell;

pub(super) fn run(args: &[String], io: &mut Io<'_>, sh: &mut Shell) -> io::Result<i32> {
    // Como o bash, o shell interativo avisa que está saindo.
    if sh.interactive {
        writeln!(io.err, "exit")?;
    }
    let args = match args.first() {
        Some(first) if first == "--" => &args[1..],
        _ => args,
    };
    let code = match args {
        [] => sh.last_status,
        [arg] => {
            if let Some(n) = parse_status(arg) {
                n
            } else {
                writeln!(
                    io.err,
                    "{}: exit: {arg}: numeric argument required",
                    sh.name
                )?;
                2
            }
        }
        [_, _, ..] => {
            writeln!(io.err, "{}: exit: too many arguments", sh.name)?;
            return Ok(1);
        }
    };
    sh.exit_request = Some(code);
    Ok(code)
}

/// Inteiro sem sinal (POSIX: "unsigned decimal integer"), com `+` e espaços
/// em volta aceitos como no dash, módulo 256. Negativo é erro, como no dash
/// (o POSIX não define o resultado, ADR-013).
fn parse_status(arg: &str) -> Option<i32> {
    let s = arg.trim_matches(|c: char| c.is_ascii_whitespace());
    let digits = s.strip_prefix('+').unwrap_or(s);
    if digits.is_empty() || !digits.bytes().all(|b| b.is_ascii_digit()) {
        return None;
    }
    let n: i64 = digits.parse().ok()?;
    i32::try_from(n.rem_euclid(256)).ok()
}

#[cfg(test)]
mod tests {
    use super::parse_status;

    #[test]
    fn status_parsing() {
        assert_eq!(parse_status("0"), Some(0));
        assert_eq!(parse_status("300"), Some(44));
        assert_eq!(parse_status("-1"), None);
        assert_eq!(parse_status("+7"), Some(7));
        assert_eq!(parse_status(" 3 "), Some(3));
        assert_eq!(parse_status("abc"), None);
        assert_eq!(parse_status("1a"), None);
        assert_eq!(parse_status("-"), None);
        assert_eq!(parse_status(""), None);
        assert_eq!(parse_status("99999999999999999999"), None);
    }
}
