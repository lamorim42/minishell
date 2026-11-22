//! De onde vêm as linhas: terminal (rustyline), texto do `-c`, arquivo de
//! script ou stdin que não é terminal.

use std::io::{BufRead, BufReader};
use std::os::fd::AsFd;

use rustyline::DefaultEditor;
use rustyline::error::ReadlineError;

/// Resultado de uma leitura.
#[derive(Debug)]
pub enum Line {
    Text(String),
    /// Ctrl+C durante a leitura.
    Interrupted,
    /// Fim da entrada (Ctrl+D na linha vazia, fim do arquivo ou do `-c`).
    Eof,
}

/// Fonte de linhas, com ou sem prompt.
pub enum Input {
    Terminal(Box<DefaultEditor>),
    /// Texto do `-c` (ou já lido), consumido linha a linha.
    Text(std::vec::IntoIter<String>),
    /// Arquivo de script, com buffer: os filhos não compartilham este fd.
    File(BufReader<std::fs::File>),
    /// stdin que não é terminal, lido byte a byte: o POSIX exige que o shell
    /// não consuma além da linha atual, porque os comandos podem ler o resto
    /// (por exemplo, `printf 'cat\nlinha\n' | minishell`).
    Stdin,
}

impl std::fmt::Debug for Input {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let kind = match self {
            Self::Terminal(_) => "Terminal",
            Self::Text(_) => "Text",
            Self::File(_) => "File",
            Self::Stdin => "Stdin",
        };
        f.write_str(kind)
    }
}

impl Input {
    pub fn text(src: &str) -> Self {
        Self::Text(
            src.split('\n')
                .map(str::to_owned)
                .collect::<Vec<_>>()
                .into_iter(),
        )
    }

    /// Lê uma linha, sem o `\n` final. `prompt` só aparece no terminal.
    pub fn read_line(&mut self, prompt: &str) -> Line {
        match self {
            Self::Terminal(editor) => match editor.readline(prompt) {
                Ok(line) => Line::Text(line),
                Err(ReadlineError::Interrupted) => Line::Interrupted,
                Err(_) => Line::Eof,
            },
            Self::Text(lines) => lines.next().map_or(Line::Eof, Line::Text),
            Self::File(reader) => {
                let mut buf = Vec::new();
                match reader.read_until(b'\n', &mut buf) {
                    Ok(0) | Err(_) => Line::Eof,
                    Ok(_) => Line::Text(finish(buf)),
                }
            }
            Self::Stdin => read_stdin_line(),
        }
    }

    pub fn add_history(&mut self, line: &str) {
        if let Self::Terminal(editor) = self
            && !line.trim().is_empty()
        {
            let _ = editor.add_history_entry(line);
        }
    }
}

fn read_stdin_line() -> Line {
    let stdin = std::io::stdin();
    let fd = stdin.as_fd();
    let mut buf = Vec::new();
    let mut byte = [0u8; 1];
    loop {
        match nix::unistd::read(fd, &mut byte) {
            Ok(0) => break,
            Ok(_) if byte[0] == b'\n' => return Line::Text(finish(buf)),
            Ok(_) => buf.push(byte[0]),
            Err(nix::errno::Errno::EINTR) => {}
            Err(_) => break,
        }
    }
    // Última linha sem `\n` ainda conta.
    if buf.is_empty() {
        Line::Eof
    } else {
        Line::Text(finish(buf))
    }
}

fn finish(mut buf: Vec<u8>) -> String {
    if buf.last() == Some(&b'\n') {
        buf.pop();
    }
    String::from_utf8_lossy(&buf).into_owned()
}
