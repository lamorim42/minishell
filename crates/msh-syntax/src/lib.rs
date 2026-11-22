//! Análise sintática do minishell: transforma uma linha de comando em AST.
//!
//! Este crate é puro: não lê arquivos, não cria processos e não consulta o
//! ambiente. A expansão (`$VAR`, `~`, field splitting) acontece depois, no
//! `msh-runtime`, a partir das partes de [`Word`].
//!
//! Referência: POSIX.1-2024, Shell Command Language — 2.2 (Quoting),
//! 2.3 (Token Recognition) e 2.10 (Shell Grammar), num subconjunto.
//!
//! ```
//! use msh_syntax::{parse, Command, Node};
//!
//! let ast = parse("echo \"oi $USER\" | wc -c").unwrap().unwrap();
//! let Node::Pipeline(pipeline) = &ast else { unreachable!() };
//! assert_eq!(pipeline.commands.len(), 2);
//! assert!(matches!(pipeline.commands[0], Command::Simple(_)));
//! // A AST volta para texto em forma canônica:
//! assert_eq!(ast.to_string(), "echo \"oi ${USER}\" | wc -c");
//! ```

pub mod ast;
mod display;
pub mod error;
pub mod lexer;
mod parser;
pub mod token;

pub use ast::{Command, Node, Pipeline, Redirect, SimpleCommand, Word, WordPart};
pub use error::ParseError;
pub use lexer::tokenize;
pub use parser::{Syntax, parse, parse_with};
pub use token::{Span, Token, TokenKind};
