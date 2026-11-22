//! Parser descendente recursivo (ADR-003), seguindo o POSIX 2.10:
//!
//! ```text
//! and_or         : pipeline (('&&' | '||') pipeline)*
//! pipeline       : command ('|' command)*
//! command        : simple_command | '(' and_or ')' redirect*
//! simple_command : (WORD | redirect)+
//! redirect       : ('<' | '>' | '>>' | '<<') WORD
//! ```

use crate::ast::{Command, Node, Pipeline, Redirect, SimpleCommand, Word};
use crate::error::ParseError;
use crate::lexer::tokenize;
use crate::token::{Span, Token, TokenKind};

/// Recursos opcionais da gramática.
///
/// O padrão é o escopo da fase 1 do minishell: `&&`, `||` e `( )` são
/// reconhecidos, mas recusados com [`ParseError::Unsupported`].
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq)]
pub struct Syntax {
    /// Aceita `&&` e `||` (POSIX 2.9.3).
    pub and_or: bool,
    /// Aceita subshell `( ... )` (POSIX 2.9.4).
    pub subshell: bool,
}

impl Syntax {
    /// Escopo da fase 1.
    pub const MINISHELL: Self = Self {
        and_or: false,
        subshell: false,
    };

    /// Toda a gramática que o parser conhece.
    pub const FULL: Self = Self {
        and_or: true,
        subshell: true,
    };
}

/// Analisa uma linha com o escopo da fase 1 ([`Syntax::MINISHELL`]).
///
/// Retorna `Ok(None)` para linha vazia ou só com comentário.
///
/// # Errors
///
/// Erro de sintaxe ou recurso fora do escopo.
pub fn parse(src: &str) -> Result<Option<Node>, ParseError> {
    parse_with(src, Syntax::MINISHELL)
}

/// Analisa uma linha com os recursos de `syntax`.
///
/// # Errors
///
/// Erro de sintaxe ou recurso desabilitado em `syntax`.
pub fn parse_with(src: &str, syntax: Syntax) -> Result<Option<Node>, ParseError> {
    let tokens = tokenize(src)?;
    if tokens.is_empty() {
        return Ok(None);
    }
    let mut parser = Parser {
        src,
        tokens,
        pos: 0,
        syntax,
    };
    let node = parser.and_or()?;
    if let Some(token) = parser.peek() {
        return Err(parser.unexpected(token));
    }
    Ok(Some(node))
}

struct Parser<'a> {
    src: &'a str,
    tokens: Vec<Token>,
    pos: usize,
    syntax: Syntax,
}

impl Parser<'_> {
    fn peek(&self) -> Option<&Token> {
        self.tokens.get(self.pos)
    }

    fn peek_kind(&self) -> Option<&TokenKind> {
        self.peek().map(|t| &t.kind)
    }

    fn bump(&mut self) -> Option<Token> {
        let token = self.tokens.get(self.pos).cloned();
        self.pos += 1;
        token
    }

    fn eof_span(&self) -> Span {
        Span::new(self.src.len(), self.src.len())
    }

    fn unexpected(&self, token: &Token) -> ParseError {
        ParseError::UnexpectedToken {
            token: token.span.slice(self.src).to_owned(),
            span: token.span,
        }
    }

    /// Erro para o token atual, ou fim de linha inesperado.
    fn unexpected_here(&self) -> ParseError {
        match self.peek() {
            Some(token) => self.unexpected(token),
            None => ParseError::UnexpectedEof {
                span: self.eof_span(),
            },
        }
    }

    fn unsupported(token: &Token, feature: &str) -> ParseError {
        ParseError::Unsupported {
            feature: feature.to_owned(),
            span: token.span,
        }
    }

    fn and_or(&mut self) -> Result<Node, ParseError> {
        let mut left = Node::Pipeline(self.pipeline()?);
        while let Some(token) = self.peek() {
            let is_and = match token.kind {
                TokenKind::AndIf => true,
                TokenKind::OrIf => false,
                _ => break,
            };
            if !self.syntax.and_or {
                return Err(Self::unsupported(token, if is_and { "&&" } else { "||" }));
            }
            self.bump();
            let right = Box::new(Node::Pipeline(self.pipeline()?));
            left = if is_and {
                Node::And(Box::new(left), right)
            } else {
                Node::Or(Box::new(left), right)
            };
        }
        Ok(left)
    }

    fn pipeline(&mut self) -> Result<Pipeline, ParseError> {
        let mut commands = vec![self.command()?];
        while matches!(self.peek_kind(), Some(TokenKind::Pipe)) {
            self.bump();
            commands.push(self.command()?);
        }
        Ok(Pipeline { commands })
    }

    fn command(&mut self) -> Result<Command, ParseError> {
        match self.peek() {
            Some(token) if token.kind == TokenKind::LParen => {
                if !self.syntax.subshell {
                    return Err(Self::unsupported(token, "("));
                }
                self.subshell()
            }
            Some(_) => self.simple_command().map(Command::Simple),
            None => Err(self.unexpected_here()),
        }
    }

    fn subshell(&mut self) -> Result<Command, ParseError> {
        self.bump(); // `(`
        let body = Box::new(self.and_or()?);
        if !matches!(self.peek_kind(), Some(TokenKind::RParen)) {
            return Err(self.unexpected_here());
        }
        self.bump(); // `)`
        let mut redirects = Vec::new();
        while self.peek_kind().is_some_and(is_redirect_op) {
            redirects.push(self.redirect()?);
        }
        Ok(Command::Subshell { body, redirects })
    }

    fn simple_command(&mut self) -> Result<SimpleCommand, ParseError> {
        let mut argv = Vec::new();
        let mut redirects = Vec::new();
        loop {
            match self.peek_kind() {
                Some(TokenKind::Word(_)) => {
                    if let Some(Token {
                        kind: TokenKind::Word(word),
                        ..
                    }) = self.bump()
                    {
                        argv.push(word);
                    }
                }
                Some(kind) if is_redirect_op(kind) => redirects.push(self.redirect()?),
                _ => break,
            }
        }
        if argv.is_empty() && redirects.is_empty() {
            return Err(self.unexpected_here());
        }
        Ok(SimpleCommand { argv, redirects })
    }

    fn redirect(&mut self) -> Result<Redirect, ParseError> {
        let op = self.bump().map(|t| t.kind);
        let target = match self.peek() {
            Some(Token {
                kind: TokenKind::Word(word),
                span,
            }) => (word.clone(), *span),
            // Como o bash: `echo >` reclama do "newline".
            None => {
                return Err(ParseError::UnexpectedToken {
                    token: "newline".to_owned(),
                    span: self.eof_span(),
                });
            }
            Some(token) => return Err(self.unexpected(token)),
        };
        self.bump();
        let (word, span) = target;
        Ok(match op {
            Some(TokenKind::Less) => Redirect::In(word),
            Some(TokenKind::Great) => Redirect::Out(word),
            Some(TokenKind::DGreat) => Redirect::Append(word),
            Some(TokenKind::DLess) => heredoc(&word, span.slice(self.src)),
            other => unreachable!("redirect() chamado com {other:?}"),
        })
    }
}

fn is_redirect_op(kind: &TokenKind) -> bool {
    matches!(
        kind,
        TokenKind::Less | TokenKind::Great | TokenKind::DGreat | TokenKind::DLess
    )
}

/// O delimitador do heredoc não sofre expansão, só remoção de aspas, e
/// qualquer aspas nele desliga a expansão das linhas (POSIX 2.7.4).
fn heredoc(word: &Word, raw: &str) -> Redirect {
    Redirect::HereDoc {
        delimiter: quote_removal(raw),
        expand: !word.is_quoted(),
    }
}

/// Remoção de aspas sobre o texto original (POSIX 2.6.7), sem expandir nada.
/// `raw` vem de um token já validado pelo lexer, então as aspas estão
/// balanceadas.
fn quote_removal(raw: &str) -> String {
    let mut out = String::with_capacity(raw.len());
    let mut chars = raw.chars().peekable();
    while let Some(c) = chars.next() {
        match c {
            '\\' => match chars.next() {
                Some('\n') => {}
                Some(next) => out.push(next),
                None => out.push('\\'),
            },
            '\'' => out.extend(chars.by_ref().take_while(|&c| c != '\'')),
            '"' => {
                while let Some(c) = chars.next() {
                    match c {
                        '"' => break,
                        '\\' => match chars.peek() {
                            Some(&next @ ('$' | '`' | '"' | '\\')) => {
                                out.push(next);
                                chars.next();
                            }
                            Some('\n') => {
                                chars.next();
                            }
                            _ => out.push('\\'),
                        },
                        c => out.push(c),
                    }
                }
            }
            c => out.push(c),
        }
    }
    out
}

#[cfg(test)]
mod tests {
    use super::quote_removal;

    #[test]
    fn quote_removal_follows_posix() {
        assert_eq!(quote_removal("EOF"), "EOF");
        assert_eq!(quote_removal("'EOF'"), "EOF");
        assert_eq!(quote_removal("\"E\"OF"), "EOF");
        assert_eq!(quote_removal(r"\EOF"), "EOF");
        assert_eq!(quote_removal("$A"), "$A");
        assert_eq!(quote_removal(r#""a\"b\n""#), r#"a"b\n"#);
        assert_eq!(quote_removal(r"a\"), r"a\");
    }
}
