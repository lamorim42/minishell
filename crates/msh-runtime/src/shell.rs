//! Estado do shell: tudo o que era global (`g_minishell`) na versão em C.

use crate::env::Env;

#[derive(Debug, Clone)]
pub struct Shell {
    pub env: Env,
    /// Status do último comando: o valor de `$?`.
    pub last_status: i32,
    /// Nome usado em `$0` e no prefixo das mensagens de erro.
    pub name: String,
}

impl Shell {
    #[must_use]
    pub fn new(env: Env) -> Self {
        Self {
            env,
            last_status: 0,
            name: "minishell".to_owned(),
        }
    }

    /// Shell com o ambiente do processo atual.
    #[must_use]
    pub fn from_os() -> Self {
        Self::new(Env::from_os())
    }

    /// Valor de um parâmetro: variável comum ou especial (POSIX 2.5.2).
    ///
    /// O minishell não tem parâmetros posicionais, então `$1`, `$#`, `$@` e
    /// `$*` valem como se a lista estivesse vazia.
    #[must_use]
    pub fn param(&self, name: &str) -> Option<String> {
        match name {
            "?" => Some(self.last_status.to_string()),
            "$" => Some(std::process::id().to_string()),
            "0" => Some(self.name.clone()),
            "#" => Some("0".to_owned()),
            "@" | "*" | "!" | "-" => None,
            n if n.starts_with(|c: char| c.is_ascii_digit()) => None,
            n => self.env.get(n).map(str::to_owned),
        }
    }
}

impl Default for Shell {
    fn default() -> Self {
        Self::new(Env::default())
    }
}
