#![allow(dead_code)]

use msh_syntax::{Command, Node, ParseError, SimpleCommand, Word, WordPart, tokenize};

/// Texto bruto de cada token, como a linha foi digitada.
pub fn raw_tokens(src: &str) -> Vec<&str> {
    tokenize(src)
        .unwrap_or_else(|e| panic!("{src:?}: {e}"))
        .iter()
        .map(|t| t.span.slice(src))
        .collect()
}

/// Palavra sem aspas e sem expansão: o que o `clean_tokens` da versão em C
/// devolvia.
pub fn text(word: &Word) -> String {
    fn part(out: &mut String, p: &WordPart) {
        match p {
            WordPart::Literal(s) | WordPart::SingleQuoted(s) => out.push_str(s),
            WordPart::DoubleQuoted(inner) => inner.iter().for_each(|p| part(out, p)),
            WordPart::Var(name) => {
                out.push('$');
                out.push_str(name);
            }
            WordPart::LastStatus => out.push_str("$?"),
            WordPart::Tilde(user) => {
                out.push('~');
                out.push_str(user.as_deref().unwrap_or(""));
            }
        }
    }
    let mut out = String::new();
    word.parts.iter().for_each(|p| part(&mut out, p));
    out
}

pub fn parse_ok(src: &str) -> Node {
    msh_syntax::parse(src)
        .unwrap_or_else(|e| panic!("{src:?}: {e}"))
        .unwrap_or_else(|| panic!("{src:?}: linha vazia"))
}

pub fn parse_err(src: &str) -> ParseError {
    match msh_syntax::parse(src) {
        Ok(ast) => panic!("{src:?}: esperava erro, veio {ast:?}"),
        Err(e) => e,
    }
}

/// O único comando simples de uma linha como `echo oi > f`.
pub fn simple(src: &str) -> SimpleCommand {
    match parse_ok(src) {
        Node::Pipeline(mut p) if p.commands.len() == 1 => match p.commands.remove(0) {
            Command::Simple(cmd) => cmd,
            other @ Command::Subshell { .. } => {
                panic!("{src:?}: esperava comando simples, veio {other:?}")
            }
        },
        other => panic!("{src:?}: esperava um comando, veio {other:?}"),
    }
}

/// Palavra com uma só parte literal.
pub fn lit(s: &str) -> Word {
    Word {
        parts: vec![WordPart::Literal(s.to_owned())],
    }
}
