//! Colunas da lista de arquivos (TDD §7): permissões no formato do `ls -l`,
//! tamanho legível e data.

/// Permissões no formato do `ls -l`, como `drwxr-xr-x`, incluindo setuid,
/// setgid e sticky bit.
#[must_use]
pub fn permissions(mode: u32) -> String {
    let kind = match mode & 0o170_000 {
        0o040_000 => 'd',
        0o120_000 => 'l',
        0o020_000 => 'c',
        0o060_000 => 'b',
        0o010_000 => 'p',
        0o140_000 => 's',
        _ => '-',
    };
    let mut out = String::with_capacity(10);
    out.push(kind);
    for (shift, special, set_char) in [(6, 0o4000, 's'), (3, 0o2000, 's'), (0, 0o1000, 't')] {
        let bits = (mode >> shift) & 0o7;
        out.push(if bits & 4 != 0 { 'r' } else { '-' });
        out.push(if bits & 2 != 0 { 'w' } else { '-' });
        let exec = bits & 1 != 0;
        out.push(match (mode & special != 0, exec) {
            (true, true) => set_char,
            (true, false) => set_char.to_ascii_uppercase(),
            (false, true) => 'x',
            (false, false) => '-',
        });
    }
    out
}

/// Tamanho em unidades do SI com uma casa e vírgula decimal, como o GNOME
/// mostra em pt-BR: `512 bytes`, `1,5 kB`, `3,2 MB`.
#[must_use]
pub fn size(bytes: u64) -> String {
    const UNITS: [&str; 6] = ["kB", "MB", "GB", "TB", "PB", "EB"];
    if bytes < 1000 {
        return if bytes == 1 {
            "1 byte".to_owned()
        } else {
            format!("{bytes} bytes")
        };
    }
    // Em décimos, com inteiros, para não depender de arredondamento de f64.
    let mut tenths = u128::from(bytes) * 10 / 1000;
    let mut unit = 0;
    while tenths >= 10_000 && unit + 1 < UNITS.len() {
        tenths /= 1000;
        unit += 1;
    }
    format!("{},{} {}", tenths / 10, tenths % 10, UNITS[unit])
}

/// Uma mensagem curta, em pt-BR, para um erro de E/S mostrado na GUI.
#[must_use]
pub fn io_error(err: &std::io::Error) -> String {
    use std::io::ErrorKind;
    match err.kind() {
        ErrorKind::PermissionDenied => "permissão negada".to_owned(),
        ErrorKind::NotFound => "não existe".to_owned(),
        ErrorKind::AlreadyExists => "já existe um item com esse nome".to_owned(),
        _ => err.to_string(),
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn ls_style_permissions() {
        assert_eq!(permissions(0o100_644), "-rw-r--r--");
        assert_eq!(permissions(0o040_755), "drwxr-xr-x");
        assert_eq!(permissions(0o041_777), "drwxrwxrwt");
        assert_eq!(permissions(0o104_755), "-rwsr-xr-x");
        assert_eq!(permissions(0o102_644), "-rw-r-Sr--");
        assert_eq!(permissions(0o120_777), "lrwxrwxrwx");
        assert_eq!(permissions(0o010_600), "prw-------");
    }

    #[test]
    fn human_sizes() {
        assert_eq!(size(0), "0 bytes");
        assert_eq!(size(1), "1 byte");
        assert_eq!(size(999), "999 bytes");
        assert_eq!(size(1000), "1,0 kB");
        assert_eq!(size(1536), "1,5 kB");
        assert_eq!(size(3_200_000), "3,2 MB");
        assert_eq!(size(999_999_999), "999,9 MB");
        assert_eq!(size(u64::MAX), "18,4 EB");
    }
}
