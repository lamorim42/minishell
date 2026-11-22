//! Lexer: quebra a linha em tokens (POSIX 2.2 e 2.3).
//!
//! Regras do subconjunto atual:
//!
//! - Espaço e tab separam palavras; `#` no início de um token começa um
//!   comentário até o fim da linha.
//! - Operadores: `|`, `||`, `&&`, `<`, `<<`, `>`, `>>`, `(` e `)`.
//! - Aspas simples, aspas duplas e `\` seguem o POSIX 2.2.
//! - `$NOME`, `${NOME}`, `$?`, `$1`, `$$`... viram partes da palavra; a
//!   expansão em si fica para o runtime.
//! - Recursos reconhecidos mas fora do escopo geram
//!   [`ParseError::Unsupported`]: `&`, `;`, nova linha, `` ` ``, `$(`, `<<-`
//!   e redirecionamento com número de fd (`2>`).

use crate::ast::{Word, WordPart};
use crate::error::ParseError;
use crate::token::{Span, Token, TokenKind};

/// Quebra `src` em tokens.
///
/// # Errors
///
/// Aspas sem fechar, `${` inválido ou recurso fora do escopo.
pub fn tokenize(src: &str) -> Result<Vec<Token>, ParseError> {
    Lexer::new(src).run()
}

/// Caracteres que encerram uma palavra sem aspas.
fn is_delimiter(c: char) -> bool {
    matches!(
        c,
        ' ' | '\t' | '\n' | '|' | '&' | ';' | '<' | '>' | '(' | ')'
    )
}

fn is_name_start(c: char) -> bool {
    c.is_ascii_alphabetic() || c == '_'
}

fn is_name_char(c: char) -> bool {
    c.is_ascii_alphanumeric() || c == '_'
}

/// Parâmetros especiais de um caractere (POSIX 2.5.2), exceto `?`, que tem
/// variante própria.
fn is_special_param(c: char) -> bool {
    matches!(c, '$' | '#' | '@' | '*' | '!' | '-' | '0'..='9')
}

/// Conteúdo aceito em `${...}`: nome, número posicional ou parâmetro especial.
fn is_valid_braced(content: &str) -> bool {
    let mut chars = content.chars();
    match (chars.next(), chars.as_str()) {
        (Some(c), rest) if is_name_start(c) => rest.chars().all(is_name_char),
        (Some(c), rest) if c.is_ascii_digit() => rest.chars().all(|c| c.is_ascii_digit()),
        (Some(c), "") => is_special_param(c) || c == '?',
        _ => false,
    }
}

/// Acumula partes de uma palavra, juntando texto vizinho do mesmo tipo.
#[derive(Debug, Default)]
struct Parts(Vec<WordPart>);

impl Parts {
    fn literal(&mut self, c: char) {
        if let Some(WordPart::Literal(s)) = self.0.last_mut() {
            s.push(c);
        } else {
            self.0.push(WordPart::Literal(c.to_string()));
        }
    }

    fn quoted(&mut self, text: &str) {
        if let Some(WordPart::SingleQuoted(s)) = self.0.last_mut() {
            s.push_str(text);
        } else {
            self.0.push(WordPart::SingleQuoted(text.to_owned()));
        }
    }

    fn push(&mut self, part: WordPart) {
        self.0.push(part);
    }
}

struct Lexer<'a> {
    src: &'a str,
    /// `(offset em bytes, caractere)`.
    chars: Vec<(usize, char)>,
    /// Índice em `chars`.
    i: usize,
    tokens: Vec<Token>,
}

impl<'a> Lexer<'a> {
    fn new(src: &'a str) -> Self {
        Self {
            src,
            chars: src.char_indices().collect(),
            i: 0,
            tokens: Vec::new(),
        }
    }

    fn peek(&self, k: usize) -> Option<char> {
        self.chars.get(self.i + k).map(|&(_, c)| c)
    }

    /// Offset em bytes do caractere de índice `i` (ou o fim da linha).
    fn offset(&self, i: usize) -> usize {
        self.chars.get(i).map_or(self.src.len(), |&(o, _)| o)
    }

    fn span_from(&self, start: usize) -> Span {
        Span::new(self.offset(start), self.offset(self.i))
    }

    fn unsupported(&self, feature: &str, start: usize, len: usize) -> ParseError {
        ParseError::Unsupported {
            feature: feature.to_owned(),
            span: Span::new(self.offset(start), self.offset(start + len)),
        }
    }

    fn run(mut self) -> Result<Vec<Token>, ParseError> {
        while let Some(c) = self.peek(0) {
            match c {
                ' ' | '\t' => self.i += 1,
                '\n' => return Err(self.unsupported("newline", self.i, 1)),
                '#' => {
                    while self.peek(0).is_some_and(|c| c != '\n') {
                        self.i += 1;
                    }
                }
                c if is_delimiter(c) => self.operator(c)?,
                _ => self.word()?,
            }
        }
        Ok(self.tokens)
    }

    fn emit(&mut self, kind: TokenKind, start: usize) {
        let span = self.span_from(start);
        self.tokens.push(Token { kind, span });
    }

    fn operator(&mut self, c: char) -> Result<(), ParseError> {
        let start = self.i;
        let next = self.peek(1);
        let (kind, len) = match (c, next) {
            ('|', Some('|')) => (TokenKind::OrIf, 2),
            ('|', _) => (TokenKind::Pipe, 1),
            ('&', Some('&')) => (TokenKind::AndIf, 2),
            ('&', _) => return Err(self.unsupported("&", start, 1)),
            (';', _) => return Err(self.unsupported(";", start, 1)),
            ('<', Some('<')) if self.peek(2) == Some('-') => {
                return Err(self.unsupported("<<-", start, 3));
            }
            ('<', Some('<')) => (TokenKind::DLess, 2),
            ('<', _) => (TokenKind::Less, 1),
            ('>', Some('>')) => (TokenKind::DGreat, 2),
            ('>', _) => (TokenKind::Great, 1),
            ('(', _) => (TokenKind::LParen, 1),
            (')', _) => (TokenKind::RParen, 1),
            _ => unreachable!("operator() chamado com {c:?}"),
        };
        self.i += len;
        self.emit(kind, start);
        Ok(())
    }

    fn word(&mut self) -> Result<(), ParseError> {
        let start = self.i;
        let mut parts = Parts::default();

        if self.peek(0) == Some('~') {
            self.tilde(&mut parts);
        }

        while let Some(c) = self.peek(0) {
            match c {
                c if is_delimiter(c) => break,
                '\'' => self.single_quoted(&mut parts)?,
                '"' => self.double_quoted(&mut parts)?,
                '\\' => match self.peek(1) {
                    // `\` no fim da linha fica literal, como no dash.
                    None => {
                        parts.quoted("\\");
                        self.i += 1;
                    }
                    // Continuação de linha: some.
                    Some('\n') => self.i += 2,
                    Some(next) => {
                        parts.quoted(next.encode_utf8(&mut [0; 4]));
                        self.i += 2;
                    }
                },
                '$' => self.dollar(&mut parts)?,
                '`' => return Err(self.unsupported("`", self.i, 1)),
                c => {
                    parts.literal(c);
                    self.i += 1;
                }
            }
        }

        // Só `\<newline>`: não sobra palavra nenhuma.
        if parts.0.is_empty() {
            return Ok(());
        }

        // `2>arquivo`: número de fd colado no operador (POSIX 2.7).
        let raw = self.span_from(start).slice(self.src);
        if raw.bytes().all(|b| b.is_ascii_digit())
            && let Some(op @ ('<' | '>')) = self.peek(0)
        {
            let feature = format!("{raw}{op}");
            return Err(self.unsupported(&feature, start, self.i - start + 1));
        }

        self.emit(TokenKind::Word(Word { parts: parts.0 }), start);
        Ok(())
    }

    /// `~` ou `~usuario` no início da palavra (POSIX 2.6.1). Se o prefixo até
    /// a primeira `/` tiver qualquer caractere com aspas ou `$`, o `~` fica
    /// literal.
    fn tilde(&mut self, parts: &mut Parts) {
        let mut j = self.i + 1;
        while let Some(&(_, c)) = self.chars.get(j) {
            if c == '/' || is_delimiter(c) {
                break;
            }
            if matches!(c, '\'' | '"' | '\\' | '$' | '`') {
                parts.literal('~');
                self.i += 1;
                return;
            }
            j += 1;
        }
        let user: String = self.chars[self.i + 1..j].iter().map(|&(_, c)| c).collect();
        parts.push(WordPart::Tilde((!user.is_empty()).then_some(user)));
        self.i = j;
    }

    fn single_quoted(&mut self, parts: &mut Parts) -> Result<(), ParseError> {
        let start = self.i;
        self.i += 1;
        let content_start = self.offset(self.i);
        loop {
            match self.peek(0) {
                None => {
                    return Err(ParseError::UnclosedQuote {
                        quote: '\'',
                        span: self.span_from(start),
                    });
                }
                Some('\'') => break,
                Some(_) => self.i += 1,
            }
        }
        parts.quoted(&self.src[content_start..self.offset(self.i)]);
        self.i += 1;
        Ok(())
    }

    fn double_quoted(&mut self, parts: &mut Parts) -> Result<(), ParseError> {
        let start = self.i;
        self.i += 1;
        let mut inner = Parts::default();
        loop {
            match self.peek(0) {
                None => {
                    return Err(ParseError::UnclosedQuote {
                        quote: '"',
                        span: self.span_from(start),
                    });
                }
                Some('"') => {
                    self.i += 1;
                    break;
                }
                // Dentro de aspas duplas, `\` só escapa `$`, `` ` ``, `"`,
                // `\` e a nova linha (POSIX 2.2.3).
                Some('\\') => match self.peek(1) {
                    Some(c @ ('$' | '`' | '"' | '\\')) => {
                        inner.literal(c);
                        self.i += 2;
                    }
                    Some('\n') => self.i += 2,
                    _ => {
                        inner.literal('\\');
                        self.i += 1;
                    }
                },
                Some('$') => self.dollar(&mut inner)?,
                Some('`') => return Err(self.unsupported("`", self.i, 1)),
                Some(c) => {
                    inner.literal(c);
                    self.i += 1;
                }
            }
        }
        parts.push(WordPart::DoubleQuoted(inner.0));
        Ok(())
    }

    /// Trata o `$` na posição atual (POSIX 2.6.2).
    fn dollar(&mut self, parts: &mut Parts) -> Result<(), ParseError> {
        let start = self.i;
        match self.peek(1) {
            Some('?') => {
                parts.push(WordPart::LastStatus);
                self.i += 2;
            }
            Some('(') => return Err(self.unsupported("$(", start, 2)),
            Some('{') => self.braced(parts)?,
            Some(c) if is_name_start(c) => {
                self.i += 1;
                let mut name = String::new();
                while let Some(c) = self.peek(0).filter(|&c| is_name_char(c)) {
                    name.push(c);
                    self.i += 1;
                }
                parts.push(WordPart::Var(name));
            }
            // `$10` é `${1}0` (POSIX 2.5.1).
            Some(c) if is_special_param(c) => {
                parts.push(WordPart::Var(c.to_string()));
                self.i += 2;
            }
            // `$` sozinho, antes de espaço, aspas, `/`... fica literal.
            // Ainda não tratamos `$'...'` (POSIX.1-2024, 2.2.4).
            _ => {
                parts.literal('$');
                self.i += 1;
            }
        }
        Ok(())
    }

    /// `${...}`, com `self.i` apontando para o `$`.
    fn braced(&mut self, parts: &mut Parts) -> Result<(), ParseError> {
        let start = self.i;
        self.i += 2;
        let content_start = self.offset(self.i);
        while self.peek(0).is_some_and(|c| c != '}') {
            self.i += 1;
        }
        if self.peek(0).is_none() {
            return Err(ParseError::BadSubstitution {
                text: self.span_from(start).slice(self.src).to_owned(),
                span: self.span_from(start),
            });
        }
        let content = &self.src[content_start..self.offset(self.i)];
        self.i += 1;
        if !is_valid_braced(content) {
            return Err(ParseError::BadSubstitution {
                text: self.span_from(start).slice(self.src).to_owned(),
                span: self.span_from(start),
            });
        }
        parts.push(if content == "?" {
            WordPart::LastStatus
        } else {
            WordPart::Var(content.to_owned())
        });
        Ok(())
    }
}
