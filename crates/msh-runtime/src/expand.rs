//! Expansão de palavras (POSIX 2.6), na ordem do padrão:
//!
//! 1. til (2.6.1);
//! 2. parâmetros: `$VAR`, `${VAR}`, `$?` e especiais (2.6.2);
//! 3. field splitting com `IFS`, só no que veio de expansão fora de aspas
//!    (2.6.5);
//! 4. expansão de caminho (2.6.6): fora do escopo da fase 1;
//! 5. remoção de aspas (2.6.7): acontece de graça, porque as partes de
//!    [`Word`] já sabem o que estava entre aspas.
//!
//! Uma palavra pode virar zero, um ou vários campos (ADR-015).

use msh_syntax::{Word, WordPart};

use crate::shell::Shell;

/// `IFS` padrão quando a variável não existe (POSIX 2.5.3).
const DEFAULT_IFS: &str = " \t\n";

#[derive(Debug, Clone, PartialEq, Eq, thiserror::Error)]
pub enum ExpandError {
    /// O alvo de um redirecionamento virou zero ou vários campos.
    #[error("{word}: ambiguous redirect")]
    AmbiguousRedirect { word: String },
}

/// Expande uma palavra em campos.
#[must_use]
pub fn expand_word(word: &Word, sh: &Shell) -> Vec<String> {
    let ifs = sh.env.get("IFS").unwrap_or(DEFAULT_IFS);
    let mut fields = Fields::new(ifs);
    for (i, part) in word.parts.iter().enumerate() {
        match part {
            WordPart::Literal(s) | WordPart::SingleQuoted(s) => fields.push_fixed(s),
            WordPart::DoubleQuoted(inner) => {
                fields.push_fixed("");
                for p in inner {
                    match p {
                        WordPart::Literal(s) => fields.push_fixed(s),
                        other => fields.push_fixed(&param_value(other, sh).unwrap_or_default()),
                    }
                }
            }
            WordPart::Var(_) | WordPart::LastStatus => {
                if let Some(value) = param_value(part, sh) {
                    fields.push_splittable(&value);
                }
            }
            // O lexer só produz `Tilde` como primeira parte.
            WordPart::Tilde(user) => {
                debug_assert_eq!(i, 0, "til fora do início da palavra");
                fields.push_fixed(&tilde(user.as_deref(), sh));
            }
        }
    }
    fields.finish()
}

/// Expande várias palavras (por exemplo, o `argv` de um comando).
#[must_use]
pub fn expand_words(words: &[Word], sh: &Shell) -> Vec<String> {
    words.iter().flat_map(|w| expand_word(w, sh)).collect()
}

/// Alvo de `<`, `>` e `>>`: precisa virar exatamente um campo.
///
/// # Errors
///
/// [`ExpandError::AmbiguousRedirect`] se a expansão der zero ou vários campos,
/// como em `> $VAZIA` ou `> $A` com `A="a b"`.
pub fn expand_redirect_target(word: &Word, sh: &Shell) -> Result<String, ExpandError> {
    let mut fields = expand_word(word, sh);
    if fields.len() == 1 {
        Ok(fields.remove(0))
    } else {
        Err(ExpandError::AmbiguousRedirect {
            word: word.to_string(),
        })
    }
}

fn param_value(part: &WordPart, sh: &Shell) -> Option<String> {
    match part {
        WordPart::Var(name) => sh.param(name),
        WordPart::LastStatus => Some(sh.last_status.to_string()),
        _ => None,
    }
}

/// `~` vira `$HOME`; `~usuario` vira a pasta pessoal do usuário. Se não der
/// para resolver, o texto fica como estava.
fn tilde(user: Option<&str>, sh: &Shell) -> String {
    let home = match user {
        None => sh.env.get("HOME").map(str::to_owned),
        Some(name) => user_home(name),
    };
    home.unwrap_or_else(|| format!("~{}", user.unwrap_or_default()))
}

fn user_home(name: &str) -> Option<String> {
    let user = nix::unistd::User::from_name(name).ok()??;
    user.dir.into_os_string().into_string().ok()
}

/// Monta os campos de uma palavra (POSIX 2.6.5).
#[derive(Debug)]
struct Fields<'a> {
    ifs: &'a str,
    done: Vec<String>,
    current: String,
    /// O campo atual já existe, mesmo vazio (por exemplo, por causa de `""`).
    started: bool,
    /// O último campo terminou em espaço do `IFS`; um separador que não é
    /// espaço logo depois pertence ao mesmo delimitador.
    after_space: bool,
}

impl<'a> Fields<'a> {
    fn new(ifs: &'a str) -> Self {
        Self {
            ifs,
            done: Vec::new(),
            current: String::new(),
            started: false,
            after_space: false,
        }
    }

    /// Texto que nunca é dividido: literal, entre aspas ou resultado do til.
    fn push_fixed(&mut self, s: &str) {
        self.current.push_str(s);
        self.started = true;
        self.after_space = false;
    }

    /// Resultado de expansão fora de aspas: passa por field splitting.
    fn push_splittable(&mut self, s: &str) {
        for c in s.chars() {
            if !self.ifs.contains(c) {
                self.current.push(c);
                self.started = true;
                self.after_space = false;
            } else if matches!(c, ' ' | '\t' | '\n') {
                if self.started {
                    self.end_field();
                    self.after_space = true;
                }
            } else {
                // Separador que não é espaço: sempre delimita um campo,
                // mesmo vazio, exceto logo depois de um espaço do IFS.
                if self.started || !self.after_space {
                    self.end_field();
                }
                self.after_space = false;
            }
        }
    }

    fn end_field(&mut self) {
        self.done.push(std::mem::take(&mut self.current));
        self.started = false;
    }

    fn finish(mut self) -> Vec<String> {
        if self.started {
            self.end_field();
        }
        self.done
    }
}
