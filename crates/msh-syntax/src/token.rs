//! Tokens produzidos pelo lexer (POSIX 2.3).

use crate::ast::Word;

/// Intervalo de bytes na linha original (`start..end`).
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Span {
    pub start: usize,
    pub end: usize,
}

impl Span {
    #[must_use]
    pub const fn new(start: usize, end: usize) -> Self {
        Self { start, end }
    }

    /// Trecho da linha original coberto por este span.
    ///
    /// # Panics
    ///
    /// Se o span não pertencer a `src` (fora dos limites ou no meio de um
    /// caractere UTF-8).
    #[must_use]
    pub fn slice(self, src: &str) -> &str {
        &src[self.start..self.end]
    }
}

/// Tipo do token.
///
/// `AndIf`, `OrIf`, `LParen` e `RParen` já são reconhecidos; o parser só os
/// aceita quando [`crate::Syntax`] habilita o recurso (ADR-014).
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum TokenKind {
    Word(Word),
    /// `|`
    Pipe,
    /// `<`
    Less,
    /// `>`
    Great,
    /// `>>`
    DGreat,
    /// `<<`
    DLess,
    /// `&&`
    AndIf,
    /// `||`
    OrIf,
    /// `(`
    LParen,
    /// `)`
    RParen,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Token {
    pub kind: TokenKind,
    pub span: Span,
}
