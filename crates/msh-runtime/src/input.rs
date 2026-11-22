//! Fonte de linhas para o que o shell precisa ler além da linha de comando:
//! hoje, o corpo dos heredocs (POSIX 2.7.4), lido da mesma entrada que os
//! comandos (terminal, `-c`, script ou stdin).

/// Resultado de uma leitura.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum ReadLine {
    /// Uma linha, sem o `\n` final.
    Text(String),
    /// Ctrl+C durante a leitura.
    Interrupted,
    /// Fim da entrada.
    Eof,
}

/// De onde o shell lê linhas. `prompt` só aparece no terminal.
pub trait LineSource {
    fn read_line(&mut self, prompt: &str) -> ReadLine;
}

/// Entrada vazia: todo heredoc termina no fim do arquivo.
#[derive(Debug, Clone, Copy, Default)]
pub struct NoInput;

impl LineSource for NoInput {
    fn read_line(&mut self, _prompt: &str) -> ReadLine {
        ReadLine::Eof
    }
}

/// Linhas já conhecidas (útil em testes e para o `-c`).
#[derive(Debug, Clone, Default)]
pub struct Lines(std::collections::VecDeque<String>);

impl Lines {
    #[must_use]
    pub fn new(text: &str) -> Self {
        Self(text.split('\n').map(str::to_owned).collect())
    }
}

impl LineSource for Lines {
    fn read_line(&mut self, _prompt: &str) -> ReadLine {
        self.0.pop_front().map_or(ReadLine::Eof, ReadLine::Text)
    }
}
