//! Variáveis do shell (ADR-007).

use std::collections::HashMap;

/// Uma variável do shell.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Var {
    /// `None` quando a variável foi só declarada com `export NOME`, sem
    /// valor: aparece no `export`, mas não no `env` nem nos filhos.
    pub value: Option<String>,
    /// Vai para o ambiente dos processos filhos.
    pub exported: bool,
}

/// Tabela de variáveis.
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct Env {
    vars: HashMap<String, Var>,
}

/// Nome válido de variável (POSIX 3.216): letras, dígitos e `_`, sem
/// começar com dígito.
#[must_use]
pub fn is_valid_name(name: &str) -> bool {
    let mut chars = name.chars();
    chars
        .next()
        .is_some_and(|c| c.is_ascii_alphabetic() || c == '_')
        && chars.all(|c| c.is_ascii_alphanumeric() || c == '_')
}

impl Env {
    /// Ambiente do processo atual. Variáveis com nome ou valor que não são
    /// UTF-8 válido ficam de fora.
    #[must_use]
    pub fn from_os() -> Self {
        Self::from_pairs(
            std::env::vars_os()
                .filter_map(|(k, v)| Some((k.into_string().ok()?, v.into_string().ok()?))),
        )
    }

    /// Ambiente a partir de pares `(nome, valor)`, todos exportados.
    pub fn from_pairs<K, V>(pairs: impl IntoIterator<Item = (K, V)>) -> Self
    where
        K: Into<String>,
        V: Into<String>,
    {
        let vars = pairs
            .into_iter()
            .map(|(k, v)| {
                let var = Var {
                    value: Some(v.into()),
                    exported: true,
                };
                (k.into(), var)
            })
            .collect();
        Self { vars }
    }

    /// Valor da variável, se ela existir e tiver valor.
    #[must_use]
    pub fn get(&self, name: &str) -> Option<&str> {
        self.vars.get(name)?.value.as_deref()
    }

    #[must_use]
    pub fn var(&self, name: &str) -> Option<&Var> {
        self.vars.get(name)
    }

    /// Define o valor, mantendo o estado de exportação.
    pub fn set(&mut self, name: impl Into<String>, value: impl Into<String>) {
        let var = self.vars.entry(name.into()).or_insert(Var {
            value: None,
            exported: false,
        });
        var.value = Some(value.into());
    }

    /// Marca como exportada e, se vier `value`, define o valor.
    pub fn export(&mut self, name: impl Into<String>, value: Option<String>) {
        let var = self.vars.entry(name.into()).or_insert(Var {
            value: None,
            exported: true,
        });
        var.exported = true;
        if value.is_some() {
            var.value = value;
        }
    }

    /// Remove a variável. Retorna `true` se ela existia.
    pub fn unset(&mut self, name: &str) -> bool {
        self.vars.remove(name).is_some()
    }

    /// Variáveis exportadas, em ordem alfabética.
    #[must_use]
    pub fn exported(&self) -> Vec<(&str, &Var)> {
        let mut list: Vec<_> = self
            .vars
            .iter()
            .filter(|(_, v)| v.exported)
            .map(|(k, v)| (k.as_str(), v))
            .collect();
        list.sort_unstable_by_key(|&(k, _)| k);
        list
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn valid_names() {
        for name in ["A", "_", "a_1", "PATH", "_x9"] {
            assert!(is_valid_name(name), "{name}");
        }
        for name in ["", "1a", "a-b", "a b", "á", "A=B", "$A"] {
            assert!(!is_valid_name(name), "{name}");
        }
    }

    #[test]
    fn set_keeps_export_flag_and_export_keeps_value() {
        let mut env = Env::default();
        env.set("A", "1");
        assert!(!env.var("A").unwrap().exported);
        env.export("A", None);
        assert_eq!(
            env.var("A"),
            Some(&Var {
                value: Some("1".into()),
                exported: true
            })
        );
        env.set("A", "2");
        assert!(env.var("A").unwrap().exported);
        assert_eq!(env.get("A"), Some("2"));
    }

    #[test]
    fn export_without_value_declares_only() {
        let mut env = Env::default();
        env.export("B", None);
        assert_eq!(env.get("B"), None);
        assert!(env.var("B").is_some());
    }

    #[test]
    fn unset_and_sorted_exported() {
        let mut env = Env::from_pairs([("Z", "1"), ("A", "2"), ("M", "3")]);
        env.set("LOCAL", "x");
        assert!(env.unset("M"));
        assert!(!env.unset("M"));
        let names: Vec<_> = env.exported().into_iter().map(|(k, _)| k).collect();
        assert_eq!(names, ["A", "Z"]);
    }
}
