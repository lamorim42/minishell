//! Erros de sintaxe, com mensagens no formato do bash (ADR-010).

use crate::token::Span;

#[derive(Debug, Clone, PartialEq, Eq, thiserror::Error)]
pub enum ParseError {
    #[error("unexpected EOF while looking for matching `{quote}'")]
    UnclosedQuote { quote: char, span: Span },

    #[error("syntax error near unexpected token `{token}'")]
    UnexpectedToken { token: String, span: Span },

    #[error("syntax error: unexpected end of file")]
    UnexpectedEof { span: Span },

    #[error("{text}: bad substitution")]
    BadSubstitution { text: String, span: Span },

    /// Recurso do POSIX reconhecido, mas fora do escopo atual.
    #[error("syntax error: `{feature}' is not supported")]
    Unsupported { feature: String, span: Span },
}

impl ParseError {
    /// Onde o erro aconteceu na linha original.
    #[must_use]
    pub fn span(&self) -> Span {
        match self {
            Self::UnclosedQuote { span, .. }
            | Self::UnexpectedToken { span, .. }
            | Self::UnexpectedEof { span }
            | Self::BadSubstitution { span, .. }
            | Self::Unsupported { span, .. } => *span,
        }
    }
}
