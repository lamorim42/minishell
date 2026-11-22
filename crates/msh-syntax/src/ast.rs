//! Árvore sintática (POSIX 2.10, subconjunto).

/// Uma palavra, dividida em partes que lembram como foram escritas.
///
/// Guardar as aspas aqui (em vez de removê-las no lexer, como na versão em C)
/// é o que permite expandir `"$A"` e `'$A'` de jeitos diferentes (ADR-004).
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Word {
    pub parts: Vec<WordPart>,
}

impl Word {
    /// `true` se alguma parte da palavra foi escrita entre aspas ou com `\`.
    #[must_use]
    pub fn is_quoted(&self) -> bool {
        self.parts
            .iter()
            .any(|p| matches!(p, WordPart::SingleQuoted(_) | WordPart::DoubleQuoted(_)))
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum WordPart {
    /// Texto sem aspas.
    Literal(String),
    /// Texto protegido por aspas simples ou por `\` fora de aspas: nunca
    /// expande e nunca sofre field splitting.
    SingleQuoted(String),
    /// Conteúdo entre aspas duplas; contém apenas `Literal`, `Var` e
    /// `LastStatus`.
    DoubleQuoted(Vec<WordPart>),
    /// `$NOME`, `${NOME}` e parâmetros especiais (`$1`, `$$`, `$#`, ...).
    Var(String),
    /// `$?`
    LastStatus,
    /// `~` ou `~usuario` no início da palavra (POSIX 2.6.1).
    Tilde(Option<String>),
}

/// Raiz da AST: uma lista AND-OR (POSIX 2.9.3).
///
/// A árvore é associativa à esquerda: `a && b || c` vira
/// `Or(And(a, b), c)`. Na fase 1 só `Pipeline` é aceito por padrão
/// (veja [`crate::Syntax`]).
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Node {
    Pipeline(Pipeline),
    And(Box<Node>, Box<Node>),
    Or(Box<Node>, Box<Node>),
}

/// `cmd1 | cmd2 | ...` (POSIX 2.9.2).
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Pipeline {
    pub commands: Vec<Command>,
}

/// Um elemento da pipeline.
///
/// O subshell fica aqui, e não em [`Node`], porque o POSIX permite usá-lo
/// dentro de uma pipeline (`(a; b) | c`) e com redirecionamentos.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Command {
    Simple(SimpleCommand),
    Subshell {
        body: Box<Node>,
        redirects: Vec<Redirect>,
    },
}

/// Palavras e redirecionamentos de um comando simples (POSIX 2.9.1).
///
/// Pode ter só redirecionamentos (`> arquivo` é válido), mas nunca fica vazio.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct SimpleCommand {
    pub argv: Vec<Word>,
    pub redirects: Vec<Redirect>,
}

/// Redirecionamentos (POSIX 2.7), na ordem em que aparecem.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Redirect {
    /// `< arquivo`
    In(Word),
    /// `> arquivo`
    Out(Word),
    /// `>> arquivo`
    Append(Word),
    /// `<< delimitador`
    HereDoc {
        /// Delimitador já sem aspas e sem expansão (POSIX 2.7.4).
        delimiter: String,
        /// `true` quando nenhuma parte do delimitador tinha aspas ou `\`:
        /// então as linhas do heredoc passam por expansão de `$`.
        expand: bool,
    },
}
