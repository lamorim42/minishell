//! Modos de entrada do binário: `-c`, script e stdin.

mod common;

use std::io::Write;
use std::process::{Command, Stdio};

use common::{MINISHELL, TempDir};

fn run(args: &[&str], stdin: &str, dir: &std::path::Path) -> (String, String, i32) {
    let mut child = Command::new(MINISHELL)
        .args(args)
        .current_dir(dir)
        .env("PATH", "/usr/bin:/bin")
        .stdin(Stdio::piped())
        .stdout(Stdio::piped())
        .stderr(Stdio::piped())
        .spawn()
        .unwrap();
    child
        .stdin
        .take()
        .unwrap()
        .write_all(stdin.as_bytes())
        .unwrap();
    let out = child.wait_with_output().unwrap();
    (
        String::from_utf8(out.stdout).unwrap(),
        String::from_utf8(out.stderr).unwrap(),
        out.status.code().unwrap(),
    )
}

#[test]
fn stdin_is_read_line_by_line() {
    // O shell não pode consumir além da linha atual: o `cat` lê o resto.
    let dir = TempDir::new();
    let (out, _, status) = run(&[], "echo a\ncat\nlido pelo cat\n", dir.path());
    assert_eq!(out, "a\nlido pelo cat\n");
    assert_eq!(status, 0);
}

#[test]
fn script_file() {
    let dir = TempDir::new();
    std::fs::write(dir.path().join("s.sh"), "echo um\nexit 4\necho nunca\n").unwrap();
    let (out, _, status) = run(&["s.sh"], "", dir.path());
    assert_eq!((out.as_str(), status), ("um\n", 4));
}

#[test]
fn missing_script_is_127() {
    let dir = TempDir::new();
    let (_, err, status) = run(&["nao_existe.sh"], "", dir.path());
    assert_eq!(err, "minishell: nao_existe.sh: No such file or directory\n");
    assert_eq!(status, 127);
}

#[test]
fn command_name_sets_dollar_zero() {
    let dir = TempDir::new();
    let (out, _, _) = run(&["-c", "echo $0", "meu-nome"], "", dir.path());
    assert_eq!(out, "meu-nome\n");
}

#[test]
fn unclosed_quote_continues_on_next_line() {
    let dir = TempDir::new();
    let (out, _, _) = run(&["-c", "echo \"a\nb\""], "", dir.path());
    assert_eq!(out, "a\nb\n");
    let (_, err, status) = run(&["-c", "echo \"a"], "", dir.path());
    assert_eq!(
        err,
        "minishell: unexpected EOF while looking for matching `\"'\n"
    );
    assert_eq!(status, 2);
}

#[test]
fn invalid_option_is_2() {
    let dir = TempDir::new();
    let (_, _, status) = run(&["-z"], "", dir.path());
    assert_eq!(status, 2);
}
