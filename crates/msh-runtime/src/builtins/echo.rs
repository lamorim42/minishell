//! `echo [-n] [texto...]`.
//!
//! O POSIX deixa `-n` e `\` a cargo da implementação, e o ADR-013 manda
//! seguir o dash nesses casos: só o primeiro argumento, se for exatamente
//! `-n`, é opção (`-nn` e um segundo `-n` são texto), e as sequências do XSI
//! (`\n`, `\t`, `\c`, `\0nnn`...) são interpretadas.

use std::io;

use super::Io;

pub(super) fn run(args: &[String], io: &mut Io<'_>) -> io::Result<i32> {
    let (newline, args) = match args.first() {
        Some(first) if first == "-n" => (false, &args[1..]),
        _ => (true, args),
    };
    let mut out = Vec::new();
    for (i, arg) in args.iter().enumerate() {
        if i > 0 {
            out.push(b' ');
        }
        if unescape(arg, &mut out) {
            // `\c`: para tudo, inclusive a quebra de linha.
            io.out.write_all(&out)?;
            return Ok(0);
        }
    }
    if newline {
        out.push(b'\n');
    }
    io.out.write_all(&out)?;
    Ok(0)
}

/// Copia `arg` para `out` interpretando as sequências do XSI. Retorna `true`
/// se encontrou `\c`.
fn unescape(arg: &str, out: &mut Vec<u8>) -> bool {
    let bytes = arg.as_bytes();
    let mut i = 0;
    while i < bytes.len() {
        let b = bytes[i];
        i += 1;
        if b != b'\\' || i == bytes.len() {
            out.push(b);
            continue;
        }
        let c = bytes[i];
        i += 1;
        match c {
            b'a' => out.push(0x07),
            b'b' => out.push(0x08),
            b'c' => return true,
            b'f' => out.push(0x0c),
            b'n' => out.push(b'\n'),
            b'r' => out.push(b'\r'),
            b't' => out.push(b'\t'),
            b'v' => out.push(0x0b),
            b'\\' => out.push(b'\\'),
            b'0' => {
                // `\0` seguido de até 3 dígitos octais.
                let mut value: u32 = 0;
                let mut digits = 0;
                while digits < 3 && i < bytes.len() && (b'0'..=b'7').contains(&bytes[i]) {
                    value = value * 8 + u32::from(bytes[i] - b'0');
                    i += 1;
                    digits += 1;
                }
                out.push(u8::try_from(value & 0xff).unwrap_or_default());
            }
            other => {
                out.push(b'\\');
                out.push(other);
            }
        }
    }
    false
}
