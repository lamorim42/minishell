//! Modo interativo num PTY real (rexpect): prompt, Ctrl+C, Ctrl+\, Ctrl+D,
//! heredoc e OSC 7 (RF-01, RF-13, RF-17; TDD §4.3).

use std::process::Command;

use rexpect::session::{PtySession, spawn_command};

const MINISHELL: &str = env!("CARGO_BIN_EXE_minishell");
const PROMPT: &str = "miau> ";

fn shell(args: &[&str]) -> PtySession {
    let mut cmd = Command::new(MINISHELL);
    cmd.args(args)
        .env_clear()
        .env("PATH", "/usr/bin:/bin")
        .env("HOME", std::env::temp_dir())
        .env("TERM", "xterm")
        .current_dir(std::env::temp_dir());
    let mut p = spawn_command(cmd, Some(10_000)).expect("abre o PTY");
    p.exp_string(PROMPT).expect("primeiro prompt");
    p
}

/// Digita uma linha (Enter é `\r` no terminal).
fn type_line(p: &mut PtySession, line: &str) {
    p.send(&format!("{line}\r")).unwrap();
    p.flush().unwrap();
}

/// Espera `text` e o próximo prompt.
fn expect_then_prompt(p: &mut PtySession, text: &str) {
    p.exp_string(text)
        .unwrap_or_else(|e| panic!("esperava {text:?}: {e}"));
    p.exp_string(PROMPT).unwrap();
}

fn exit_status(p: &mut PtySession) -> i32 {
    p.exp_eof().unwrap();
    // O rexpect usa outra versão do nix; lemos o status pelo Debug,
    // `Exited(Pid(123), 4)`.
    let status = format!("{:?}", p.process_mut().wait().unwrap());
    let code = status
        .strip_prefix("Exited(")
        .and_then(|s| s.rsplit_once(", "))
        .and_then(|(_, c)| c.trim_end_matches(')').parse().ok());
    code.unwrap_or_else(|| panic!("terminou com {status}"))
}

#[test]
fn prompt_runs_commands() {
    let mut p = shell(&[]);
    type_line(&mut p, "echo oi | tr a-z A-Z");
    expect_then_prompt(&mut p, "OI");
    type_line(&mut p, "exit 4");
    p.exp_string("exit").unwrap();
    assert_eq!(exit_status(&mut p), 4);
}

#[test]
fn ctrl_c_at_prompt_gives_new_prompt_and_130() {
    let mut p = shell(&[]);
    p.send("texto pela metade").unwrap();
    p.send_control('c').unwrap();
    p.exp_string(PROMPT).unwrap();
    type_line(&mut p, "echo status=$?");
    expect_then_prompt(&mut p, "status=130");
}

#[test]
fn ctrl_c_kills_only_the_command() {
    let mut p = shell(&[]);
    type_line(&mut p, "sleep 10");
    std::thread::sleep(std::time::Duration::from_millis(300));
    p.send_control('c').unwrap();
    p.exp_string(PROMPT).unwrap();
    type_line(&mut p, "echo status=$?");
    expect_then_prompt(&mut p, "status=130");
}

#[test]
fn ctrl_c_kills_a_whole_pipeline() {
    let mut p = shell(&[]);
    type_line(&mut p, "sleep 10 | sleep 10");
    std::thread::sleep(std::time::Duration::from_millis(300));
    p.send_control('c').unwrap();
    p.exp_string(PROMPT).unwrap();
    type_line(&mut p, "echo status=$?");
    expect_then_prompt(&mut p, "status=130");
}

#[test]
fn ctrl_backslash_quits_the_command_with_131() {
    let mut p = shell(&[]);
    type_line(&mut p, "sleep 10");
    std::thread::sleep(std::time::Duration::from_millis(300));
    p.send_control('\\').unwrap();
    p.exp_string("Quit").unwrap();
    p.exp_string(PROMPT).unwrap();
    type_line(&mut p, "echo status=$?");
    expect_then_prompt(&mut p, "status=131");
}

#[test]
fn ctrl_backslash_at_prompt_is_ignored() {
    let mut p = shell(&[]);
    p.send_control('\\').unwrap();
    // O terminal descarta a entrada pendente ao gerar o sinal (sem NOFLSH).
    std::thread::sleep(std::time::Duration::from_millis(300));
    type_line(&mut p, "echo vivo");
    expect_then_prompt(&mut p, "vivo");
}

#[test]
fn ctrl_d_prints_exit_and_keeps_status() {
    let mut p = shell(&[]);
    type_line(&mut p, "sh -c 'exit 3'");
    p.exp_string(PROMPT).unwrap();
    p.send_control('d').unwrap();
    p.exp_string("exit").unwrap();
    assert_eq!(exit_status(&mut p), 3);
}

#[test]
fn heredoc_reads_from_terminal() {
    let mut p = shell(&[]);
    type_line(&mut p, "cat <<FIM");
    p.exp_string("> ").unwrap();
    type_line(&mut p, "linha do heredoc");
    // O rustyline descarta o que foi digitado antes de voltar ao modo raw,
    // então esperamos cada prompt antes de digitar.
    p.exp_string("> ").unwrap();
    type_line(&mut p, "FIM");
    p.exp_string("FIM").unwrap();
    expect_then_prompt(&mut p, "linha do heredoc");
}

#[test]
fn ctrl_c_in_heredoc_cancels_the_line() {
    let mut p = shell(&[]);
    type_line(&mut p, "cat <<FIM > nao_deve_existir_msh");
    p.exp_string("> ").unwrap();
    p.send_control('c').unwrap();
    p.exp_string(PROMPT).unwrap();
    type_line(&mut p, "echo status=$?");
    expect_then_prompt(&mut p, "status=130");
    assert!(!std::env::temp_dir().join("nao_deve_existir_msh").exists());
}

#[test]
fn unclosed_quote_asks_for_more() {
    let mut p = shell(&[]);
    type_line(&mut p, "echo \"a");
    p.exp_string("> ").unwrap();
    type_line(&mut p, "b\"");
    expect_then_prompt(&mut p, "b");
}

#[test]
fn osc7_reports_directory() {
    let mut p = shell(&["--osc7"]);
    type_line(&mut p, "cd /usr");
    p.exp_string("\x1b]7;file://").unwrap();
    p.exp_string("/usr\x07").unwrap();
}
