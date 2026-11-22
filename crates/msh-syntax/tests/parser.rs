mod common;

use common::{lit, parse_err, parse_ok, simple, text};
use msh_syntax::{Command, Node, ParseError, Pipeline, Redirect, Syntax, parse, parse_with};

fn full(src: &str) -> Node {
    parse_with(src, Syntax::FULL)
        .unwrap_or_else(|e| panic!("{src:?}: {e}"))
        .unwrap()
}

fn full_err(src: &str) -> ParseError {
    parse_with(src, Syntax::FULL).expect_err(src)
}

fn unexpected(src: &str) -> String {
    match parse_with(src, Syntax::FULL) {
        Err(ParseError::UnexpectedToken { token, .. }) => token,
        other => panic!("{src:?}: esperava UnexpectedToken, veio {other:?}"),
    }
}

// --- Casos portados de tests/tests_sintax_analysis.cpp ---------------------

#[test]
fn ported_syntax_cases() {
    assert!(parse("ls | ls").is_ok());
    assert_eq!(unexpected("| ls"), "|");
    for op in [">", ">>", "<", "<<"] {
        assert_eq!(unexpected(&format!("{op} |")), "|", "{op}");
    }
    assert_eq!(unexpected("ls | | ls"), "|");
}

#[test]
fn trailing_pipe_is_an_error() {
    // A versão em C aceitava `ls |`; o POSIX e o bash -c recusam.
    assert!(matches!(
        parse_err("ls |"),
        ParseError::UnexpectedEof { .. }
    ));
    assert_eq!(
        parse_err("ls |").to_string(),
        "syntax error: unexpected end of file"
    );
}

// --- Linhas vazias ---------------------------------------------------------

#[test]
fn empty_lines_parse_to_none() {
    for src in ["", "   ", "\t", "# comentário", "  # x | y"] {
        assert_eq!(parse(src), Ok(None), "{src:?}");
    }
}

// --- Comandos simples e redirecionamentos (POSIX 2.7 e 2.9.1) --------------

#[test]
fn redirects_keep_order_and_leave_argv() {
    let cmd = simple("< in cat -e > out >> log");
    assert_eq!(cmd.argv, [lit("cat"), lit("-e")]);
    assert_eq!(
        cmd.redirects,
        [
            Redirect::In(lit("in")),
            Redirect::Out(lit("out")),
            Redirect::Append(lit("log")),
        ]
    );
}

#[test]
fn redirect_only_command_is_valid() {
    let cmd = simple("> out");
    assert!(cmd.argv.is_empty());
    assert_eq!(cmd.redirects, [Redirect::Out(lit("out"))]);
}

#[test]
fn redirect_without_target() {
    assert_eq!(unexpected("echo >"), "newline");
    assert_eq!(
        parse_err("echo >").to_string(),
        "syntax error near unexpected token `newline'"
    );
    assert_eq!(unexpected("echo > > f"), ">");
}

#[test]
fn heredoc_delimiter() {
    let cases = [
        ("cat << EOF", "EOF", true),
        ("cat <<EOF", "EOF", true),
        ("cat << 'EOF'", "EOF", false),
        ("cat <<E\"O\"F", "EOF", false),
        (r"cat << \EOF", "EOF", false),
        // Sem expansão no delimitador, mesmo sem aspas.
        ("cat << $A", "$A", true),
        ("cat << ''", "", false),
    ];
    for (src, delimiter, expand) in cases {
        assert_eq!(
            simple(src).redirects,
            [Redirect::HereDoc {
                delimiter: delimiter.to_owned(),
                expand
            }],
            "{src:?}"
        );
    }
}

#[test]
fn pipeline_of_simple_commands() {
    let Node::Pipeline(Pipeline { commands }) = parse_ok("ls -a | tr a b | wc -l") else {
        panic!("esperava pipeline");
    };
    let names: Vec<_> = commands
        .iter()
        .map(|c| match c {
            Command::Simple(s) => text(&s.argv[0]),
            Command::Subshell { .. } => panic!("subshell inesperado"),
        })
        .collect();
    assert_eq!(names, ["ls", "tr", "wc"]);
}

#[test]
fn complex_line_snapshot() {
    insta::assert_debug_snapshot!(parse_ok(
        "< in grep -v \"a $B\" | sort -r > \"out file\" >> ~/log"
    ));
}

// --- Escopo da fase 1 (Syntax::MINISHELL) ----------------------------------

#[test]
fn minishell_scope_rejects_and_or_and_subshell() {
    for (src, feature) in [
        ("a && b", "&&"),
        ("a || b", "||"),
        ("(a)", "("),
        ("a | (b)", "("),
    ] {
        match parse_err(src) {
            ParseError::Unsupported { feature: f, .. } => assert_eq!(f, feature, "{src:?}"),
            other => panic!("{src:?}: esperava Unsupported, veio {other:?}"),
        }
    }
}

// --- Gramática completa (Syntax::FULL) -------------------------------------

#[test]
fn and_or_is_left_associative() {
    let ast = full("a && b || c");
    let Node::Or(left, right) = ast else {
        panic!("esperava Or no topo")
    };
    assert!(matches!(*left, Node::And(_, _)));
    assert!(matches!(*right, Node::Pipeline(_)));
}

#[test]
fn subshell_in_pipeline_with_redirects_snapshot() {
    insta::assert_debug_snapshot!(full("(a && b | c) > out | d"));
}

#[test]
fn full_syntax_errors() {
    assert!(matches!(full_err("a &&"), ParseError::UnexpectedEof { .. }));
    assert!(matches!(full_err("(a"), ParseError::UnexpectedEof { .. }));
    assert!(matches!(
        full_err("(a | b"),
        ParseError::UnexpectedEof { .. }
    ));
    assert_eq!(unexpected("()"), ")");
    assert_eq!(unexpected("a)"), ")");
    assert_eq!(unexpected("(a) b"), "b");
    assert_eq!(unexpected("echo (a)"), "(");
    assert_eq!(unexpected("&& a"), "&&");
}

// --- Forma canônica (Display) ----------------------------------------------

#[test]
fn canonical_form() {
    let cases = [
        ("echo   oi", "echo oi"),
        ("< in cat>out", "cat < in > out"),
        ("echo \"a $B\"'c'", "echo \"a ${B}\"'c'"),
        ("echo it\\'s", "echo it''\\'''s"),
        ("echo ${A}b $?", "echo ${A}b $?"),
        ("cat << 'E F'", "cat << 'E F'"),
        ("cat << EOF", "cat << EOF"),
        ("echo ~/x ~ana", "echo ~/x ~ana"),
    ];
    for (src, expected) in cases {
        assert_eq!(parse_ok(src).to_string(), expected, "{src:?}");
    }
    assert_eq!(full("(a&&b)>o|c||d").to_string(), "(a && b) > o | c || d");
}
