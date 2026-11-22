//! OSC 7: avisa o terminal do diretório atual a cada prompt
//! (`ESC ] 7 ; file://host/caminho BEL`). A GUI da fase 2 usa isso para
//! seguir o `cd` do shell (TDD §7, RF-17).

use std::io::Write;

/// Liga o OSC 7 com `--osc7` ou dentro de um terminal VTE (que define
/// `VTE_VERSION` e entende a sequência).
#[must_use]
pub fn wanted(flag: bool) -> bool {
    flag || std::env::var_os("VTE_VERSION").is_some()
}

/// Sequência para `dir`, com o caminho codificado como URI (RFC 3986).
#[must_use]
pub fn sequence(host: &str, dir: &str) -> String {
    format!("\x1b]7;file://{}{}\x07", encode(host), encode(dir))
}

/// Escreve a sequência do diretório atual em `out`.
pub fn emit(out: &mut impl Write, pwd: Option<&str>) {
    let dir = pwd.map_or_else(
        || {
            std::env::current_dir()
                .map(|p| p.to_string_lossy().into_owned())
                .unwrap_or_default()
        },
        str::to_owned,
    );
    let host = nix::unistd::gethostname()
        .ok()
        .and_then(|h| h.into_string().ok())
        .unwrap_or_default();
    let _ = out.write_all(sequence(&host, &dir).as_bytes());
    let _ = out.flush();
}

fn encode(s: &str) -> String {
    use std::fmt::Write as _;

    let mut out = String::with_capacity(s.len());
    for b in s.bytes() {
        if b.is_ascii_alphanumeric() || b"/-._~".contains(&b) {
            out.push(char::from(b));
        } else {
            let _ = write!(out, "%{b:02X}");
        }
    }
    out
}

#[cfg(test)]
mod tests {
    use super::sequence;

    #[test]
    fn encodes_path() {
        assert_eq!(
            sequence("pc", "/home/ana/meus arquivos"),
            "\x1b]7;file://pc/home/ana/meus%20arquivos\x07"
        );
        assert_eq!(sequence("pc", "/tmp/ç"), "\x1b]7;file://pc/tmp/%C3%A7\x07");
    }
}
