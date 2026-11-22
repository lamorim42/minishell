//! URIs `file://host/caminho` do OSC 7 (RF-22). O `msh-cli` codifica o
//! caminho em `osc7.rs`; aqui fazemos o inverso para a GUI.

use std::ffi::OsString;
use std::os::unix::ffi::OsStringExt;
use std::path::PathBuf;

/// O caminho de uma URI `file://`. O host é ignorado: o shell roda na mesma
/// máquina que a GUI. Devolve `None` para outros esquemas, caminhos
/// relativos ou escapes `%` inválidos.
#[must_use]
pub fn file_uri_path(uri: &str) -> Option<PathBuf> {
    let rest = uri.strip_prefix("file://")?;
    let path = &rest[rest.find('/')?..];
    let bytes = percent_decode(path)?;
    Some(PathBuf::from(OsString::from_vec(bytes)))
}

/// A URI `file://` de um caminho absoluto (sem host), com os mesmos escapes
/// do `osc7.rs`.
#[must_use]
pub fn path_file_uri(path: &std::path::Path) -> String {
    use std::fmt::Write as _;

    let mut out = String::from("file://");
    for &b in path.as_os_str().as_encoded_bytes() {
        if b.is_ascii_alphanumeric() || b"/-._~".contains(&b) {
            out.push(char::from(b));
        } else {
            let _ = write!(out, "%{b:02X}");
        }
    }
    out
}

fn percent_decode(s: &str) -> Option<Vec<u8>> {
    let mut out = Vec::with_capacity(s.len());
    let mut bytes = s.bytes();
    while let Some(b) = bytes.next() {
        if b == b'%' {
            let hi = char::from(bytes.next()?).to_digit(16)?;
            let lo = char::from(bytes.next()?).to_digit(16)?;
            out.push(u8::try_from(hi * 16 + lo).ok()?);
        } else {
            out.push(b);
        }
    }
    Some(out)
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::path::Path;

    #[test]
    fn decodes_what_the_shell_sends() {
        assert_eq!(
            file_uri_path("file://pc/home/ana/meus%20arquivos"),
            Some(PathBuf::from("/home/ana/meus arquivos"))
        );
        assert_eq!(
            file_uri_path("file:///tmp/%C3%A7"),
            Some(PathBuf::from("/tmp/ç"))
        );
        assert_eq!(file_uri_path("file://pc/"), Some(PathBuf::from("/")));
    }

    #[test]
    fn rejects_other_uris() {
        assert_eq!(file_uri_path("http://pc/tmp"), None);
        assert_eq!(file_uri_path("file://pc"), None);
        assert_eq!(file_uri_path("file:///a%2"), None);
        assert_eq!(file_uri_path("file:///a%zz"), None);
    }

    #[test]
    fn round_trip() {
        for p in ["/", "/tmp/a b", "/tmp/it's", "/tmp/ação/100%"] {
            assert_eq!(
                file_uri_path(&path_file_uri(Path::new(p))),
                Some(PathBuf::from(p))
            );
        }
    }
}
