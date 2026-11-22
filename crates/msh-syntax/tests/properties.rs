//! Propriedades do lexer e do parser, com entradas geradas pelo proptest.

use msh_syntax::{Syntax, parse_with, tokenize};
use proptest::prelude::*;

/// Alfabeto pequeno e cheio de caracteres especiais do shell, para que boa
/// parte das linhas geradas seja válida.
const SHELL_ISH: &str = r#"[ab ~/$?{}#'"\\|&<>()2_=.-]{0,32}"#;

proptest! {
    #![proptest_config(ProptestConfig::with_cases(2_000))]

    /// Nenhuma entrada derruba o lexer ou o parser.
    #[test]
    fn never_panics(src in any::<String>()) {
        let _ = tokenize(&src);
        let _ = parse_with(&src, Syntax::FULL);
    }

    #[test]
    fn never_panics_on_shell_like_input(src in SHELL_ISH) {
        let _ = parse_with(&src, Syntax::FULL);
    }

    /// Imprimir a AST e analisar de novo devolve a mesma AST.
    #[test]
    fn display_round_trips(src in SHELL_ISH) {
        if let Ok(Some(ast)) = parse_with(&src, Syntax::FULL) {
            let printed = ast.to_string();
            let reparsed = parse_with(&printed, Syntax::FULL);
            prop_assert_eq!(reparsed, Ok(Some(ast)), "original: {:?}, impresso: {:?}", src, printed);
        }
    }

    /// Spans ficam dentro da linha e em ordem.
    #[test]
    fn spans_are_ordered_and_in_bounds(src in SHELL_ISH) {
        if let Ok(tokens) = tokenize(&src) {
            let mut last_end = 0;
            for t in &tokens {
                prop_assert!(last_end <= t.span.start);
                prop_assert!(t.span.start < t.span.end);
                prop_assert!(t.span.end <= src.len());
                last_end = t.span.end;
            }
        }
    }
}
