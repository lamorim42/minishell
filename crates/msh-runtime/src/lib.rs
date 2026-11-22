//! Runtime do minishell: estado do shell, expansão e builtins.
//!
//! Situação por marco:
//!
//! - M2: [`Env`], [`Shell`], expansão de palavras (POSIX 2.6) e os
//!   builtins `env`, `export` e `unset`.
//! - M3: execução de comando simples ([`Shell::execute_line`]), `PATH`,
//!   atribuições, `echo`, `cd`, `pwd` e `exit`.
//! - M4: pipes, redirecionamentos e heredoc.

pub mod builtins;
pub mod env;
pub mod exec;
pub mod expand;
pub mod shell;

pub use builtins::{Builtin, Io};
pub use env::{Env, Var};
pub use expand::{
    ExpandError, expand_assignment, expand_redirect_target, expand_word, expand_words,
};
pub use shell::Shell;
