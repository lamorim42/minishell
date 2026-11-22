//! Voltar e avançar na barra de caminho (TDD §7).

use std::path::{Path, PathBuf};

/// Histórico de navegação, como o de um navegador: ir para uma pasta nova
/// descarta o que estava "à frente".
#[derive(Debug, Clone)]
pub struct History {
    back: Vec<PathBuf>,
    current: PathBuf,
    forward: Vec<PathBuf>,
}

impl History {
    #[must_use]
    pub fn new(start: PathBuf) -> Self {
        Self {
            back: Vec::new(),
            current: start,
            forward: Vec::new(),
        }
    }

    #[must_use]
    pub fn current(&self) -> &Path {
        &self.current
    }

    /// Vai para `path`. Devolve `false` (e não muda nada) se já estiver lá,
    /// o que evita laços entre o OSC 7 do shell e o `cd` da tela.
    pub fn visit(&mut self, path: PathBuf) -> bool {
        if path == self.current {
            return false;
        }
        self.back.push(std::mem::replace(&mut self.current, path));
        self.forward.clear();
        true
    }

    pub fn go_back(&mut self) -> Option<&Path> {
        let prev = self.back.pop()?;
        self.forward
            .push(std::mem::replace(&mut self.current, prev));
        Some(&self.current)
    }

    pub fn go_forward(&mut self) -> Option<&Path> {
        let next = self.forward.pop()?;
        self.back.push(std::mem::replace(&mut self.current, next));
        Some(&self.current)
    }

    #[must_use]
    pub fn can_go_back(&self) -> bool {
        !self.back.is_empty()
    }

    #[must_use]
    pub fn can_go_forward(&self) -> bool {
        !self.forward.is_empty()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn back_and_forward() {
        let mut h = History::new("/".into());
        assert!(!h.can_go_back());
        assert!(h.visit("/etc".into()));
        assert!(h.visit("/etc/apt".into()));
        assert!(!h.visit("/etc/apt".into()), "mesma pasta não entra");
        assert_eq!(h.go_back(), Some(Path::new("/etc")));
        assert_eq!(h.go_back(), Some(Path::new("/")));
        assert_eq!(h.go_back(), None);
        assert_eq!(h.go_forward(), Some(Path::new("/etc")));
        assert!(h.visit("/tmp".into()));
        assert!(!h.can_go_forward(), "ir para outra pasta limpa o avançar");
        assert_eq!(h.current(), Path::new("/tmp"));
    }
}
