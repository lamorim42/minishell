//! Builtins do M2: `env`, `export` e `unset`.

use msh_runtime::{Builtin, Env, Io, Shell};

struct Run {
    status: i32,
    out: String,
    err: String,
}

fn run(sh: &mut Shell, name: &str, args: &[&str]) -> Run {
    let builtin = Builtin::lookup(name).expect("builtin existe");
    let args: Vec<String> = args.iter().map(|&a| a.to_owned()).collect();
    let (mut out, mut err) = (Vec::new(), Vec::new());
    let status = builtin.run(
        &args,
        &mut Io {
            out: &mut out,
            err: &mut err,
        },
        sh,
    );
    Run {
        status,
        out: String::from_utf8(out).unwrap(),
        err: String::from_utf8(err).unwrap(),
    }
}

fn shell(vars: &[(&str, &str)]) -> Shell {
    Shell::new(Env::from_pairs(vars.iter().copied()))
}

#[test]
fn lookup() {
    assert_eq!(Builtin::lookup("export"), Some(Builtin::Export));
    assert_eq!(Builtin::lookup("ls"), None);
}

// --- env -------------------------------------------------------------------

#[test]
fn env_lists_exported_with_value_sorted() {
    let mut sh = shell(&[("B", "2"), ("A", "1")]);
    sh.env.set("LOCAL", "nao aparece");
    sh.env.export("SEM_VALOR", None);
    let r = run(&mut sh, "env", &[]);
    assert_eq!(
        (r.status, r.out.as_str(), r.err.as_str()),
        (0, "A=1\nB=2\n", "")
    );
}

#[test]
fn env_rejects_arguments() {
    let r = run(&mut shell(&[]), "env", &["ls"]);
    assert_eq!(r.status, 125);
    assert_eq!(
        r.err,
        "minishell: env: `ls': options and arguments are not supported\n"
    );
}

// --- export ----------------------------------------------------------------

#[test]
fn export_sets_and_marks() {
    let mut sh = shell(&[]);
    sh.env.set("LOCAL", "x");
    let r = run(&mut sh, "export", &["A=1", "LOCAL", "B", "C=a=b", "D="]);
    assert_eq!(r.status, 0);
    assert_eq!(sh.env.get("A"), Some("1"));
    assert!(sh.env.var("LOCAL").unwrap().exported);
    assert_eq!(sh.env.get("B"), None);
    assert_eq!(sh.env.get("C"), Some("a=b"));
    assert_eq!(sh.env.get("D"), Some(""));
}

#[test]
fn export_without_args_lists_in_posix_format() {
    let mut sh = shell(&[("A", "it's \"$x\""), ("B", "")]);
    sh.env.export("C", None);
    let expected = "export A=\"it's \\\"\\$x\\\"\"\nexport B=\"\"\nexport C\n";
    assert_eq!(run(&mut sh, "export", &[]).out, expected);
    assert_eq!(run(&mut sh, "export", &["-p"]).out, expected);
}

#[test]
fn export_invalid_identifier_keeps_going() {
    let mut sh = shell(&[]);
    let r = run(&mut sh, "export", &["1A=x", "OK=1", "=x", "A-B"]);
    assert_eq!(r.status, 1);
    assert_eq!(sh.env.get("OK"), Some("1"));
    assert_eq!(
        r.err,
        "minishell: export: `1A=x': not a valid identifier\n\
         minishell: export: `=x': not a valid identifier\n\
         minishell: export: `A-B': not a valid identifier\n"
    );
}

#[test]
fn export_invalid_option() {
    let r = run(&mut shell(&[]), "export", &["-x"]);
    assert_eq!(r.status, 2);
    assert_eq!(r.err, "minishell: export: -x: invalid option\n");
}

// --- unset -----------------------------------------------------------------

#[test]
fn unset_removes() {
    let mut sh = shell(&[("A", "1"), ("B", "2")]);
    let r = run(&mut sh, "unset", &["A", "NAO_EXISTE"]);
    assert_eq!(r.status, 0);
    assert_eq!(sh.env.get("A"), None);
    assert_eq!(sh.env.get("B"), Some("2"));
    assert_eq!(run(&mut sh, "unset", &["-v", "B"]).status, 0);
    assert_eq!(sh.env.get("B"), None);
}

/// O POSIX trata nome inválido como erro (status > 0), como faz o dash
/// (`bad variable name`, status 2). O bash 5.2 aceita em silêncio com status
/// 0. Seguimos o padrão, com a mensagem e o status do `export` (ADR-013).
#[test]
fn unset_invalid() {
    let mut sh = shell(&[("A", "1")]);
    let r = run(&mut sh, "unset", &["1x", "A"]);
    assert_eq!(r.status, 1);
    assert_eq!(r.err, "minishell: unset: `1x': not a valid identifier\n");
    assert_eq!(sh.env.get("A"), None);
    assert_eq!(run(&mut sh, "unset", &["-f", "x"]).status, 2);
}

// --- M3: echo e exit -----------------------------------------------------------

#[test]
fn echo_options_and_escapes() {
    let mut sh = shell(&[]);
    assert_eq!(run(&mut sh, "echo", &["a", "b"]).out, "a b\n");
    assert_eq!(run(&mut sh, "echo", &[]).out, "\n");
    assert_eq!(run(&mut sh, "echo", &["-n", "x"]).out, "x");
    assert_eq!(run(&mut sh, "echo", &["-nn", "x"]).out, "-nn x\n");
    assert_eq!(run(&mut sh, "echo", &["a\\tb\\\\"]).out, "a\tb\\\n");
    assert_eq!(run(&mut sh, "echo", &["a\\cb", "c"]).out, "a");
    assert_eq!(run(&mut sh, "echo", &["\\0101\\q"]).out, "A\\q\n");
}

#[test]
fn exit_sets_request() {
    let mut sh = shell(&[]);
    sh.last_status = 3;
    assert_eq!(run(&mut sh, "exit", &[]).status, 3);
    assert_eq!(sh.exit_request.take(), Some(3));

    assert_eq!(run(&mut sh, "exit", &["300"]).status, 44);
    assert_eq!(sh.exit_request.take(), Some(44));

    let r = run(&mut sh, "exit", &["abc"]);
    assert_eq!(r.status, 2);
    assert_eq!(r.err, "minishell: exit: abc: numeric argument required\n");
    assert_eq!(sh.exit_request.take(), Some(2));

    let r = run(&mut sh, "exit", &["1", "2"]);
    assert_eq!(r.status, 1);
    assert_eq!(r.err, "minishell: exit: too many arguments\n");
    assert_eq!(sh.exit_request, None);
}

#[test]
fn interactive_exit_prints_exit() {
    let mut sh = shell(&[]);
    sh.interactive = true;
    assert_eq!(run(&mut sh, "exit", &[]).err, "exit\n");
}

#[test]
fn special_builtins() {
    for name in ["exit", "export", "unset"] {
        assert!(Builtin::lookup(name).unwrap().is_special(), "{name}");
    }
    for name in ["cd", "echo", "env", "pwd"] {
        assert!(!Builtin::lookup(name).unwrap().is_special(), "{name}");
    }
}
