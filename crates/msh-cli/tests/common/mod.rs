//! Roda o mesmo script no minishell, no dash e no `bash --posix` (ADR-012).
//!
//! Cada caso roda num diretório temporário novo, com ambiente mínimo
//! (`PATH`, `HOME`, `LC_ALL=C`). Comparamos stdout e status com os dois
//! oráculos, e stderr com o do bash, sem o prefixo do nome do shell (o
//! formato das mensagens segue o bash, ADR-010).
//!
//! Se o dash e o bash divergirem, o caso precisa de expectativa explícita
//! ([`Case::expect`]), com comentário citando a seção do POSIX.
#![allow(dead_code)]

use std::path::{Path, PathBuf};
use std::process::{Command, Output, Stdio};
use std::sync::atomic::{AtomicUsize, Ordering};

pub const MINISHELL: &str = env!("CARGO_BIN_EXE_minishell");

/// Resultado de um shell.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Run {
    pub stdout: String,
    pub stderr: String,
    pub status: i32,
}

/// Diretório temporário apagado no `Drop`.
#[derive(Debug)]
pub struct TempDir(PathBuf);

impl TempDir {
    pub fn new() -> Self {
        static N: AtomicUsize = AtomicUsize::new(0);
        let dir = std::env::temp_dir().join(format!(
            "msh-compat-{}-{}",
            std::process::id(),
            N.fetch_add(1, Ordering::Relaxed)
        ));
        std::fs::create_dir_all(&dir).unwrap();
        // Caminho canônico, para o $PWD bater entre os shells.
        Self(dir.canonicalize().unwrap())
    }

    pub fn path(&self) -> &Path {
        &self.0
    }
}

impl Drop for TempDir {
    fn drop(&mut self) {
        let _ = std::fs::remove_dir_all(&self.0);
    }
}

/// Roda `program args... -c script` no diretório `dir`.
pub fn run_in(dir: &Path, program: &str, args: &[&str], script: &str, stdin: &str) -> Run {
    let mut cmd = Command::new(program);
    cmd.args(args)
        .arg("-c")
        .arg(script)
        .current_dir(dir)
        .env_clear()
        .env("PATH", "/usr/local/bin:/usr/bin:/bin")
        .env("HOME", dir)
        .env("PWD", dir)
        .env("LC_ALL", "C")
        .stdin(Stdio::piped())
        .stdout(Stdio::piped())
        .stderr(Stdio::piped());
    let mut child = cmd.spawn().unwrap_or_else(|e| panic!("{program}: {e}"));
    {
        use std::io::Write;
        let mut pipe = child.stdin.take().unwrap();
        let _ = pipe.write_all(stdin.as_bytes());
    }
    let Output {
        status,
        stdout,
        stderr,
    } = child.wait_with_output().unwrap();
    Run {
        stdout: String::from_utf8_lossy(&stdout).into_owned(),
        stderr: normalize_stderr(&String::from_utf8_lossy(&stderr)),
        status: status.code().unwrap_or(-1),
    }
}

/// Tira o prefixo de cada linha de erro: `minishell: `, `bash: line 1: `,
/// `bash: -c: line 1: `, `dash: 1: `.
pub fn normalize_stderr(s: &str) -> String {
    s.lines()
        .map(|line| {
            for prefix in ["minishell: ", "bash: ", "dash: "] {
                if let Some(rest) = line.strip_prefix(prefix) {
                    let rest = rest.strip_prefix("-c: ").unwrap_or(rest);
                    let rest = strip_line_number(rest);
                    return rest.to_owned();
                }
            }
            line.to_owned()
        })
        .collect::<Vec<_>>()
        .join("\n")
}

fn strip_line_number(s: &str) -> &str {
    // `line 12: ` (bash) ou `12: ` (dash)
    let t = s.strip_prefix("line ").unwrap_or(s);
    let digits = t.bytes().take_while(u8::is_ascii_digit).count();
    if digits > 0 && t[digits..].starts_with(": ") {
        &t[digits + 2..]
    } else {
        s
    }
}

fn have(program: &str) -> bool {
    Command::new(program)
        .arg("-c")
        .arg("exit 0")
        .stdout(Stdio::null())
        .stderr(Stdio::null())
        .status()
        .is_ok_and(|s| s.success())
}

/// Um caso de compatibilidade.
#[allow(missing_debug_implementations)]
pub struct Case<'a> {
    script: &'a str,
    stdin: &'a str,
    setup: Option<&'a dyn Fn(&Path)>,
    expect: Option<(&'a str, i32)>,
    check_stderr: bool,
}

pub fn case(script: &str) -> Case<'_> {
    Case {
        script,
        stdin: "",
        setup: None,
        expect: None,
        check_stderr: true,
    }
}

impl<'a> Case<'a> {
    /// Prepara o diretório temporário antes de cada shell rodar.
    pub fn setup(mut self, f: &'a dyn Fn(&Path)) -> Self {
        self.setup = Some(f);
        self
    }

    pub fn stdin(mut self, stdin: &'a str) -> Self {
        self.stdin = stdin;
        self
    }

    /// Expectativa explícita (stdout, status), para quando os oráculos
    /// divergem entre si ou do POSIX. Os oráculos não são consultados.
    pub fn expect(mut self, stdout: &'a str, status: i32) -> Self {
        self.expect = Some((stdout, status));
        self
    }

    /// Não compara stderr (mensagens que o bash formata de outro jeito).
    pub fn any_stderr(mut self) -> Self {
        self.check_stderr = false;
        self
    }

    fn run_one(&self, program: &str, args: &[&str]) -> Run {
        let dir = TempDir::new();
        if let Some(setup) = self.setup {
            setup(dir.path());
        }
        let run = run_in(dir.path(), program, args, self.script, self.stdin);
        // Caminhos do diretório temporário variam entre execuções.
        let tmp = dir.path().to_string_lossy().into_owned();
        Run {
            stdout: run.stdout.replace(&tmp, "$TMP"),
            stderr: run.stderr.replace(&tmp, "$TMP"),
            status: run.status,
        }
    }

    /// Roda e confere. Devolve o resultado do minishell.
    #[track_caller]
    pub fn check(self) -> Run {
        let ours = self.run_one(MINISHELL, &[]);
        if let Some((stdout, status)) = self.expect {
            assert_eq!(
                (ours.stdout.as_str(), ours.status),
                (stdout, status),
                "script: {:?}\nstderr: {}",
                self.script,
                ours.stderr
            );
            return ours;
        }
        let mut compared = false;
        if have("dash") {
            let dash = self.run_one("dash", &[]);
            assert_eq!(
                (&ours.stdout, ours.status),
                (&dash.stdout, dash.status),
                "minishell x dash\nscript: {:?}\nours stderr: {}\ndash stderr: {}",
                self.script,
                ours.stderr,
                dash.stderr
            );
            compared = true;
        }
        if have("bash") {
            let bash = self.run_one("bash", &["--posix"]);
            assert_eq!(
                (&ours.stdout, ours.status),
                (&bash.stdout, bash.status),
                "minishell x bash --posix\nscript: {:?}\nours stderr: {}\nbash stderr: {}",
                self.script,
                ours.stderr,
                bash.stderr
            );
            if self.check_stderr {
                assert_eq!(
                    ours.stderr, bash.stderr,
                    "stderr: minishell x bash --posix\nscript: {:?}",
                    self.script
                );
            }
            compared = true;
        }
        if !compared {
            eprintln!(
                "aviso: sem dash nem bash, caso não comparado: {:?}",
                self.script
            );
        }
        ours
    }
}
