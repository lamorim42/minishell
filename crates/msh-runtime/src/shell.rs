//! Estado do shell: tudo o que era global (`g_minishell`) na versão em C.

use std::path::Path;

use crate::env::Env;

#[derive(Debug, Clone)]
pub struct Shell {
    pub env: Env,
    /// Status do último comando: o valor de `$?`.
    pub last_status: i32,
    /// Nome usado em `$0` e no prefixo das mensagens de erro.
    pub name: String,
    /// Shell interativo (prompt no terminal). Muda o que acontece com erros:
    /// um shell não interativo sai em erro de sintaxe e em erro de builtin
    /// especial (POSIX 2.8.1).
    pub interactive: bool,
    /// Pedido de saída feito pelo `exit` (ou por um erro que encerra o
    /// shell não interativo), com o código de saída.
    pub exit_request: Option<i32>,
}

impl Shell {
    #[must_use]
    pub fn new(env: Env) -> Self {
        Self {
            env,
            last_status: 0,
            name: "minishell".to_owned(),
            interactive: false,
            exit_request: None,
        }
    }

    /// Shell com o ambiente do processo atual.
    ///
    /// Como manda o POSIX (`cd`, seção *Environment Variables*, e 2.5.3), o
    /// `PWD` herdado só é mantido se for um caminho absoluto para o
    /// diretório atual; senão, recebe o caminho físico.
    #[must_use]
    pub fn from_os() -> Self {
        let mut sh = Self::new(Env::from_os());
        let inherited_ok = sh.env.get("PWD").is_some_and(pwd_is_valid);
        if !inherited_ok && let Ok(cwd) = std::env::current_dir() {
            sh.env
                .export("PWD", Some(cwd.to_string_lossy().into_owned()));
        }
        sh
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

    /// Escreve `minishell: <msg>` em stderr.
    pub fn error(&self, msg: impl std::fmt::Display) {
        eprintln!("{}: {msg}", self.name);
    }
}

/// `PWD` aceitável: absoluto, sem `.` nem `..`, e apontando para o mesmo
/// diretório que `.`.
pub(crate) fn pwd_is_valid(pwd: &str) -> bool {
    use std::os::unix::fs::MetadataExt;

    let path = Path::new(pwd);
    if !path.is_absolute() || pwd.split('/').any(|c| c == "." || c == "..") {
        return false;
    }
    match (std::fs::metadata(path), std::fs::metadata(".")) {
        (Ok(a), Ok(b)) => a.dev() == b.dev() && a.ino() == b.ino(),
        _ => false,
    }
}

impl Default for Shell {
    fn default() -> Self {
        Self::new(Env::default())
    }
}
