//! Forma canônica: a AST impressa de volta como shell.
//!
//! Garante `parse(&ast.to_string()) == ast` (testado com proptest), o que
//! serve para depurar o parser e, mais tarde, para o histórico.

use std::fmt::{self, Display, Formatter, Write};

use crate::ast::{Command, Node, Pipeline, Redirect, SimpleCommand, Word, WordPart};

impl Display for Node {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        match self {
            Self::Pipeline(p) => p.fmt(f),
            Self::And(l, r) => write!(f, "{l} && {r}"),
            Self::Or(l, r) => write!(f, "{l} || {r}"),
        }
    }
}

impl Display for Pipeline {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        for (i, cmd) in self.commands.iter().enumerate() {
            if i > 0 {
                f.write_str(" | ")?;
            }
            cmd.fmt(f)?;
        }
        Ok(())
    }
}

impl Display for Command {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        match self {
            Self::Simple(cmd) => cmd.fmt(f),
            Self::Subshell { body, redirects } => {
                write!(f, "({body})")?;
                for r in redirects {
                    write!(f, " {r}")?;
                }
                Ok(())
            }
        }
    }
}

impl Display for SimpleCommand {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        let words = self.argv.iter().map(|w| w as &dyn Display);
        let redirects = self.redirects.iter().map(|r| r as &dyn Display);
        for (i, item) in words.chain(redirects).enumerate() {
            if i > 0 {
                f.write_char(' ')?;
            }
            item.fmt(f)?;
        }
        Ok(())
    }
}

impl Display for Redirect {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        match self {
            Self::In(w) => write!(f, "< {w}"),
            Self::Out(w) => write!(f, "> {w}"),
            Self::Append(w) => write!(f, ">> {w}"),
            // Sem aspas, o delimitador é o próprio texto original.
            Self::HereDoc {
                delimiter,
                expand: true,
            } => write!(f, "<< {delimiter}"),
            Self::HereDoc {
                delimiter,
                expand: false,
            } => {
                f.write_str("<< ")?;
                write_single_quoted(f, delimiter)
            }
        }
    }
}

impl Display for Word {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        for part in &self.parts {
            write_part(f, part)?;
        }
        Ok(())
    }
}

fn write_part(f: &mut Formatter<'_>, part: &WordPart) -> fmt::Result {
    match part {
        WordPart::Literal(s) => f.write_str(s),
        WordPart::SingleQuoted(s) => write_single_quoted(f, s),
        WordPart::DoubleQuoted(inner) => {
            f.write_char('"')?;
            for p in inner {
                match p {
                    WordPart::Literal(s) => {
                        for c in s.chars() {
                            if matches!(c, '"' | '\\' | '$' | '`') {
                                f.write_char('\\')?;
                            }
                            f.write_char(c)?;
                        }
                    }
                    other => write_part(f, other)?,
                }
            }
            f.write_char('"')
        }
        // Sempre com chaves, para `${A}b` não virar `$Ab`.
        WordPart::Var(name) => write!(f, "${{{name}}}"),
        WordPart::LastStatus => f.write_str("$?"),
        WordPart::Tilde(None) => f.write_char('~'),
        WordPart::Tilde(Some(user)) => write!(f, "~{user}"),
    }
}

/// `it's` vira `'it'\''s'`.
fn write_single_quoted(f: &mut Formatter<'_>, s: &str) -> fmt::Result {
    f.write_char('\'')?;
    for c in s.chars() {
        if c == '\'' {
            f.write_str("'\\''")?;
        } else {
            f.write_char(c)?;
        }
    }
    f.write_char('\'')
}
