//! `minishell`: o miaushell em Rust.
//!
//! ```text
//! minishell                  interativo se stdin e stderr forem terminais
//! minishell -c 'comando' [nome]
//! minishell [-i] script
//! ```
//!
//! O shell é interativo com `-i`, ou sem operandos e com stdin e stderr num
//! terminal (POSIX, `sh`, opção `-i`). Só então aparece o prompt `miau> `.

mod input;

use std::ffi::OsString;
use std::io::BufReader;
use std::ops::ControlFlow;
use std::process::ExitCode;

use msh_runtime::Shell;
use msh_runtime::exec::{STATUS_NOT_FOUND, STATUS_SYNTAX, sys};
use msh_syntax::ParseError;

use crate::input::{Input, Line};

const PROMPT: &str = "miau> ";
/// Prompt de continuação (o `PS2` do POSIX).
const PROMPT_MORE: &str = "> ";
/// `$?` depois de Ctrl+C no prompt (128 + SIGINT).
const STATUS_INTERRUPTED: i32 = 130;

const USAGE: &str = "usage: minishell [-i] [-c command [name] | script]";

#[derive(Debug, PartialEq, Eq)]
enum Mode {
    Command { src: String, name: Option<String> },
    Script(String),
    Stdin,
}

#[derive(Debug, PartialEq, Eq)]
struct Options {
    mode: Mode,
    interactive: bool,
}

fn main() -> ExitCode {
    let args: Vec<OsString> = std::env::args_os().skip(1).collect();
    let mut sh = Shell::from_os();
    let opts = match parse_args(&args) {
        Ok(Some(opts)) => opts,
        Ok(None) => return ExitCode::SUCCESS,
        Err(msg) => {
            sh.error(msg);
            eprintln!("{USAGE}");
            return exit_code(STATUS_SYNTAX);
        }
    };

    let mut input = match opts.mode {
        Mode::Command { src, name } => {
            if let Some(name) = name {
                sh.name = name;
            }
            Input::text(&src)
        }
        Mode::Script(path) => match std::fs::File::open(&path) {
            Ok(file) => Input::File(BufReader::new(file)),
            Err(e) => {
                sh.error(format_args!("{path}: {}", strerror(&e)));
                return exit_code(STATUS_NOT_FOUND);
            }
        },
        Mode::Stdin if opts.interactive => match rustyline::DefaultEditor::new() {
            Ok(editor) => Input::Terminal(Box::new(editor)),
            Err(e) => {
                sh.error(format_args!("terminal: {e}"));
                Input::Stdin
            }
        },
        Mode::Stdin => Input::Stdin,
    };
    sh.interactive = opts.interactive;
    if sh.interactive {
        // ADR-008: o pai ignora Ctrl+C e Ctrl+\ enquanto um comando roda; o
        // rustyline lê essas teclas em modo raw, e os filhos voltam ao padrão.
        sys::ignore_job_signals();
    }
    exit_code(run(&mut sh, &mut input))
}

/// Lê e executa até o fim da entrada ou um `exit`. Devolve o código de saída.
fn run(sh: &mut Shell, input: &mut Input) -> i32 {
    loop {
        let mut line = match input.read_line(PROMPT) {
            Line::Text(line) => line,
            Line::Interrupted => {
                sh.last_status = STATUS_INTERRUPTED;
                continue;
            }
            Line::Eof => {
                if sh.interactive {
                    eprintln!("exit");
                }
                return sh.last_status;
            }
        };
        // Aspas abertas continuam na próxima linha (POSIX 2.2.2 e 2.2.3).
        while needs_more(&line) {
            match input.read_line(PROMPT_MORE) {
                Line::Text(more) => {
                    line.push('\n');
                    line.push_str(&more);
                }
                Line::Interrupted => {
                    line.clear();
                    sh.last_status = STATUS_INTERRUPTED;
                    break;
                }
                // Sem mais entrada: o parser vai reclamar da aspa aberta.
                Line::Eof => break,
            }
        }
        input.add_history(&line);
        if let ControlFlow::Break(code) = sh.execute_line(&line) {
            return code;
        }
    }
}

fn needs_more(line: &str) -> bool {
    matches!(
        msh_syntax::tokenize(line),
        Err(ParseError::UnclosedQuote { .. })
    )
}

fn parse_args(args: &[OsString]) -> Result<Option<Options>, String> {
    let mut interactive = None;
    let mut command = false;
    let mut i = 0;
    while let Some(arg) = args.get(i) {
        let Some(arg) = arg.to_str() else { break };
        match arg {
            "-c" => command = true,
            "-i" => interactive = Some(true),
            "--" => {
                i += 1;
                break;
            }
            "--help" => {
                println!("{USAGE}");
                return Ok(None);
            }
            "--version" => {
                println!("minishell {}", env!("CARGO_PKG_VERSION"));
                return Ok(None);
            }
            opt if opt.starts_with('-') && opt.len() > 1 => {
                return Err(format!("{opt}: invalid option"));
            }
            _ => break,
        }
        i += 1;
    }
    let operands: Vec<String> = args[i..]
        .iter()
        .map(|a| a.to_string_lossy().into_owned())
        .collect();

    let mode = if command {
        let mut operands = operands.into_iter();
        let src = operands
            .next()
            .ok_or_else(|| "-c: option requires an argument".to_owned())?;
        Mode::Command {
            src,
            name: operands.next(),
        }
    } else if let Some(script) = operands.into_iter().next() {
        Mode::Script(script)
    } else {
        Mode::Stdin
    };
    let interactive = interactive.unwrap_or_else(|| mode == Mode::Stdin && on_terminal());
    Ok(Some(Options { mode, interactive }))
}

fn on_terminal() -> bool {
    nix::unistd::isatty(std::io::stdin()).unwrap_or(false)
        && nix::unistd::isatty(std::io::stderr()).unwrap_or(false)
}

fn strerror(e: &std::io::Error) -> String {
    e.raw_os_error().map_or_else(
        || e.to_string(),
        |code| nix::errno::Errno::from_raw(code).desc().to_owned(),
    )
}

fn exit_code(code: i32) -> ExitCode {
    ExitCode::from(u8::try_from(code.rem_euclid(256)).unwrap_or(1))
}

#[cfg(test)]
mod tests {
    use super::*;

    fn parse(args: &[&str]) -> Result<Option<Options>, String> {
        let args: Vec<OsString> = args.iter().map(OsString::from).collect();
        parse_args(&args)
    }

    #[test]
    fn command_mode() {
        let opts = parse(&["-c", "echo oi", "nome"]).unwrap().unwrap();
        assert_eq!(
            opts.mode,
            Mode::Command {
                src: "echo oi".into(),
                name: Some("nome".into())
            }
        );
        assert!(!opts.interactive);
    }

    #[test]
    fn script_and_errors() {
        let opts = parse(&["-i", "x.sh"]).unwrap().unwrap();
        assert_eq!(opts.mode, Mode::Script("x.sh".into()));
        assert!(opts.interactive);
        assert!(parse(&["-c"]).is_err());
        assert!(parse(&["-x"]).is_err());
    }

    #[test]
    fn exit_codes_wrap() {
        assert_eq!(exit_code(256), ExitCode::from(0));
        assert_eq!(exit_code(-1), ExitCode::from(255));
    }
}
