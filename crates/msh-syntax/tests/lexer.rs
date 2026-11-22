mod common;

use common::{raw_tokens, simple, text};
use msh_syntax::{ParseError, Span, TokenKind, Word, WordPart, tokenize};

use WordPart::{DoubleQuoted, LastStatus, Literal, SingleQuoted, Tilde, Var};

fn parts(src: &str) -> Vec<WordPart> {
    let mut tokens = tokenize(src).unwrap_or_else(|e| panic!("{src:?}: {e}"));
    assert_eq!(tokens.len(), 1, "{src:?} deveria ter um só token");
    match tokens.remove(0).kind {
        TokenKind::Word(Word { parts }) => parts,
        other => panic!("{src:?}: esperava palavra, veio {other:?}"),
    }
}

fn lexer_err(src: &str) -> ParseError {
    tokenize(src).expect_err(src)
}

fn s(text: &str) -> String {
    text.to_owned()
}

// --- Casos portados de tests/tests_tokenizer.cpp ---------------------------

#[test]
fn ported_tokenizer_cases() {
    let cases: &[(&str, &[&str])] = &[
        ("miau um dois", &["miau", "um", "dois"]),
        (
            "ls -a | tr a b | wc -l",
            &["ls", "-a", "|", "tr", "a", "b", "|", "wc", "-l"],
        ),
        ("ls|cat -e", &["ls", "|", "cat", "-e"]),
        ("ls|cat-e", &["ls", "|", "cat-e"]),
        ("\"l\"s | cat -e", &["\"l\"s", "|", "cat", "-e"]),
        ("\"ls -a\"", &["\"ls -a\""]),
        ("\"ls\" -a", &["\"ls\"", "-a"]),
        ("l\"s\" -a", &["l\"s\"", "-a"]),
        (
            "echo \"hello world shell\"",
            &["echo", "\"hello world shell\""],
        ),
        (
            "ca\"t\"|\" cat\"<out",
            &["ca\"t\"", "|", "\" cat\"", "<", "out"],
        ),
        ("'ls' -a", &["'ls'", "-a"]),
        ("'ls -a'", &["'ls -a'"]),
        (
            "echo \"abba 'ebba' obba\"",
            &["echo", "\"abba 'ebba' obba\""],
        ),
        ("echo oi > fileout", &["echo", "oi", ">", "fileout"]),
        ("ls >file", &["ls", ">", "file"]),
        ("ls >> file", &["ls", ">>", "file"]),
        ("ls >>file", &["ls", ">>", "file"]),
        ("cat <fileout", &["cat", "<", "fileout"]),
        ("cat << fileout", &["cat", "<<", "fileout"]),
        ("cat <<fileout", &["cat", "<<", "fileout"]),
        ("cat<<fileout", &["cat", "<<", "fileout"]),
        (
            "echo \"$HOME\" \"$USER\"",
            &["echo", "\"$HOME\"", "\"$USER\""],
        ),
        (
            "echo \"\"$HOME\"\" ''$USER''",
            &["echo", "\"\"$HOME\"\"", "''$USER''"],
        ),
        (
            "echo 'oi '$USER', \"bem vindo\"'",
            &["echo", "'oi '$USER', \"bem vindo\"'"],
        ),
        (
            "echo $USER | l's' -l\"a\"",
            &["echo", "$USER", "|", "l's'", "-l\"a\""],
        ),
        ("<<o", &["<<", "o"]),
        ("<<\"o\"", &["<<", "\"o\""]),
    ];
    for (src, expected) in cases {
        assert_eq!(raw_tokens(src), *expected, "{src:?}");
    }
}

#[test]
fn ported_unclosed_quote_is_an_error() {
    // Em C o tokenizer devolvia o resto da linha; pelo POSIX é erro.
    assert_eq!(
        lexer_err("echo \"abba"),
        ParseError::UnclosedQuote {
            quote: '"',
            span: Span::new(5, 10)
        }
    );
}

// --- Casos portados de tests/tests_lexical_analyzer.cpp --------------------

/// Os rótulos que a versão em C guardava em `char **lex`.
fn labels(src: &str) -> Vec<&'static str> {
    tokenize(src)
        .unwrap()
        .into_iter()
        .map(|t| match t.kind {
            TokenKind::Word(w) if matches!(w.parts.as_slice(), [Var(_)]) => "VAR",
            TokenKind::Word(_) => "WORD",
            TokenKind::Pipe => "PIPE",
            TokenKind::Great => "REDO",
            TokenKind::DGreat => "REDA",
            TokenKind::Less => "REDI",
            TokenKind::DLess => "HERE",
            other => panic!("rótulo sem equivalente em C: {other:?}"),
        })
        .collect()
}

#[test]
fn ported_lexical_analyzer_cases() {
    let cases: &[(&str, &[&str])] = &[
        ("ls", &["WORD"]),
        ("ls |", &["WORD", "PIPE"]),
        ("ls | ls", &["WORD", "PIPE", "WORD"]),
        ("ls > file | ls", &["WORD", "REDO", "WORD", "PIPE", "WORD"]),
        ("ls >> file", &["WORD", "REDA", "WORD"]),
        ("cat < file", &["WORD", "REDI", "WORD"]),
        ("echo $USER", &["WORD", "VAR"]),
        ("cat << miau", &["WORD", "HERE", "WORD"]),
    ];
    for (src, expected) in cases {
        assert_eq!(labels(src), *expected, "{src:?}");
    }
}

// --- Casos portados de tests/tests_clean_tokens.cpp ------------------------

#[test]
fn ported_quote_removal_cases() {
    let cases = [
        ("\"ls\"", "ls"),
        ("\"l\"s", "ls"),
        ("l\"s\"", "ls"),
        ("ls\"\"", "ls"),
        ("ls", "ls"),
        ("'ls'", "ls"),
        ("\"'ls'\"", "'ls'"),
        ("'\"ls\"'", "\"ls\""),
        ("'l's", "ls"),
        ("'l'\"s\"", "ls"),
    ];
    for (src, expected) in cases {
        assert_eq!(text(&simple(src).argv[0]), expected, "{src:?}");
    }
}

// --- Aspas e escapes (POSIX 2.2) -------------------------------------------

#[test]
fn quotes_are_kept_as_parts() {
    assert_eq!(
        parts("a\"b $C\"'d'"),
        [
            Literal(s("a")),
            DoubleQuoted(vec![Literal(s("b ")), Var(s("C"))]),
            SingleQuoted(s("d")),
        ]
    );
    assert_eq!(parts("''"), [SingleQuoted(s(""))]);
    assert_eq!(parts("\"\""), [DoubleQuoted(vec![])]);
    // Aspas simples vizinhas se juntam: o sentido é o mesmo.
    assert_eq!(parts("'a''b'"), [SingleQuoted(s("ab"))]);
}

#[test]
fn backslash_outside_quotes_quotes_one_char() {
    assert_eq!(
        parts(r"a\ b"),
        [Literal(s("a")), SingleQuoted(s(" ")), Literal(s("b"))]
    );
    assert_eq!(parts(r"\$HOME"), [SingleQuoted(s("$")), Literal(s("HOME"))]);
    assert_eq!(
        parts(r"it\'s"),
        [Literal(s("it")), SingleQuoted(s("'")), Literal(s("s"))]
    );
    // No fim da linha, fica literal (como no dash).
    assert_eq!(parts(r"a\"), [Literal(s("a")), SingleQuoted(s(r"\"))]);
    // Continuação de linha some.
    assert_eq!(parts("a\\\nb"), [Literal(s("ab"))]);
}

#[test]
fn backslash_inside_double_quotes_only_escapes_some_chars() {
    assert_eq!(
        parts(r#""\$A \" \\ \n""#),
        [DoubleQuoted(vec![Literal(s(r#"$A " \ \n"#))])]
    );
}

#[test]
fn single_quotes_never_expand() {
    assert_eq!(parts("'$HOME ~ \\'"), [SingleQuoted(s("$HOME ~ \\"))]);
}

// --- Parâmetros (POSIX 2.5 e 2.6.2) ----------------------------------------

#[test]
fn dollar_forms() {
    assert_eq!(parts("$HOME"), [Var(s("HOME"))]);
    assert_eq!(parts("$?"), [LastStatus]);
    assert_eq!(parts("${?}"), [LastStatus]);
    assert_eq!(parts("${HOME}x"), [Var(s("HOME")), Literal(s("x"))]);
    assert_eq!(parts("$A_1-x"), [Var(s("A_1")), Literal(s("-x"))]);
    assert_eq!(parts("$10"), [Var(s("1")), Literal(s("0"))]);
    assert_eq!(parts("${10}"), [Var(s("10"))]);
    assert_eq!(parts("$$"), [Var(s("$"))]);
    assert_eq!(parts("$#"), [Var(s("#"))]);
}

#[test]
fn lone_dollar_is_literal() {
    assert_eq!(parts("$"), [Literal(s("$"))]);
    assert_eq!(parts("a$/b"), [Literal(s("a$/b"))]);
    assert_eq!(parts("\"$\""), [DoubleQuoted(vec![Literal(s("$"))])]);
    assert_eq!(parts("$'x'"), [Literal(s("$")), SingleQuoted(s("x"))]);
}

#[test]
fn bad_substitution() {
    for src in ["${A", "${A B}", "${A:-x}", "${}", "${1a}"] {
        assert!(
            matches!(lexer_err(src), ParseError::BadSubstitution { .. }),
            "{src:?}"
        );
    }
    assert_eq!(
        lexer_err("echo ${A B}").to_string(),
        "${A B}: bad substitution"
    );
}

// --- Til (POSIX 2.6.1) -----------------------------------------------------

#[test]
fn tilde_only_at_word_start() {
    assert_eq!(parts("~"), [Tilde(None)]);
    assert_eq!(parts("~/x"), [Tilde(None), Literal(s("/x"))]);
    assert_eq!(parts("~ana/x"), [Tilde(Some(s("ana"))), Literal(s("/x"))]);
    assert_eq!(parts("a~"), [Literal(s("a~"))]);
    assert_eq!(parts("'~'"), [SingleQuoted(s("~"))]);
    // Prefixo com aspas: o `~` fica literal.
    assert_eq!(parts("~'x'"), [Literal(s("~")), SingleQuoted(s("x"))]);
    assert_eq!(parts("~$A"), [Literal(s("~")), Var(s("A"))]);
}

// --- Comentários e espaços (POSIX 2.3) -------------------------------------

#[test]
fn comments() {
    assert_eq!(raw_tokens("echo a # resto"), ["echo", "a"]);
    assert_eq!(raw_tokens("echo a#b"), ["echo", "a#b"]);
    assert_eq!(raw_tokens("   # só comentário"), Vec::<&str>::new());
    assert_eq!(raw_tokens(" \t "), Vec::<&str>::new());
}

#[test]
fn spans_point_to_the_source() {
    let tokens = tokenize("echo  'oi' >>f").unwrap();
    let spans: Vec<_> = tokens.iter().map(|t| (t.span.start, t.span.end)).collect();
    assert_eq!(spans, [(0, 4), (6, 10), (11, 13), (13, 14)]);
}

#[test]
fn utf8_is_handled_by_char() {
    assert_eq!(
        raw_tokens("echo 'olá' ação|ç"),
        ["echo", "'olá'", "ação", "|", "ç"]
    );
}

// --- Fora do escopo --------------------------------------------------------

#[test]
fn unsupported_features_are_reported() {
    let cases = [
        ("sleep 1 &", "&"),
        ("a ; b", ";"),
        ("echo `ls`", "`"),
        ("echo \"`ls`\"", "`"),
        ("echo $(ls)", "$("),
        ("cat <<-EOF", "<<-"),
        ("ls 2>err", "2>"),
        ("cat 0<in", "0<"),
        ("a\nb", "newline"),
    ];
    for (src, feature) in cases {
        match lexer_err(src) {
            ParseError::Unsupported { feature: f, .. } => assert_eq!(f, feature, "{src:?}"),
            other => panic!("{src:?}: esperava Unsupported, veio {other:?}"),
        }
    }
    // Número separado por espaço é argumento comum.
    assert_eq!(raw_tokens("echo 2 > f"), ["echo", "2", ">", "f"]);
    assert_eq!(
        lexer_err("a ; b").to_string(),
        "syntax error: `;' is not supported"
    );
}
