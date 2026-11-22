//! Execução (POSIX 2.9): da linha de texto ao status de saída.
//!
//! [`Shell::execute_line`] é a fachada (o `facade.c` da versão em C):
//! analisa, expande e executa, e grava o `$?`.

pub mod heredoc;
pub mod path;
pub mod redirect;
pub mod sys;

use std::ffi::CString;
use std::io::Write;
use std::ops::ControlFlow;
use std::os::fd::{AsFd, OwnedFd};
use std::os::unix::ffi::OsStrExt;

use msh_syntax::{Command, Node, Pipeline, SimpleCommand, Word, WordPart};
use nix::errno::Errno;
use nix::fcntl::OFlag;

use crate::builtins::{Builtin, Io};
use crate::env::is_valid_name;
use crate::expand::{expand_assignment, expand_words};
use crate::input::LineSource;
use crate::shell::Shell;

/// Status de erro de sintaxe e de uso (POSIX 2.8.2).
pub const STATUS_SYNTAX: i32 = 2;
/// Comando achado, mas não executável (POSIX 2.8.2).
pub const STATUS_NOT_EXECUTABLE: i32 = 126;
/// Comando não encontrado (POSIX 2.8.2).
pub const STATUS_NOT_FOUND: i32 = 127;
/// Linha cancelada com Ctrl+C (128 + SIGINT).
pub const STATUS_INTERRUPTED: i32 = 130;

/// Onde um comando simples roda.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum Place {
    /// No processo do shell: comando sozinho na pipeline.
    Shell,
    /// Num filho já criado (estágio de uma pipeline, POSIX 2.9.2).
    Child,
}

impl Shell {
    /// Analisa e executa uma linha. Os heredocs são lidos de `input`.
    ///
    /// Retorna `Break(código)` quando o shell deve sair: `exit`, ou um erro
    /// que encerra o shell não interativo (erro de sintaxe ou de builtin
    /// especial, POSIX 2.8.1).
    pub fn execute_line(&mut self, line: &str, input: &mut dyn LineSource) -> ControlFlow<i32> {
        match msh_syntax::parse(line) {
            Ok(None) => {}
            Ok(Some(node)) => self.last_status = self.execute(&node, input),
            Err(e) => {
                self.error(&e);
                self.last_status = STATUS_SYNTAX;
                if !self.interactive {
                    self.exit_request = Some(STATUS_SYNTAX);
                }
            }
        }
        match self.exit_request.take() {
            Some(code) => ControlFlow::Break(code),
            None => ControlFlow::Continue(()),
        }
    }

    /// Executa uma árvore já analisada e devolve o status.
    pub fn execute(&mut self, node: &Node, input: &mut dyn LineSource) -> i32 {
        match node {
            Node::Pipeline(pipeline) => self.run_pipeline(pipeline, input),
            Node::And(..) | Node::Or(..) => {
                self.error("&& and ||: not supported");
                STATUS_SYNTAX
            }
        }
    }

    fn run_pipeline(&mut self, pipeline: &Pipeline, input: &mut dyn LineSource) -> i32 {
        // Heredocs primeiro, no pai, na ordem em que aparecem (ADR-009).
        let Ok(heredocs) = heredoc::read_all(pipeline, self, input) else {
            return STATUS_INTERRUPTED;
        };
        match (pipeline.commands.as_slice(), heredocs) {
            ([Command::Simple(cmd)], mut heredocs) => {
                let fds = heredocs.pop().unwrap_or_default();
                self.run_simple(cmd, fds, Place::Shell)
            }
            (commands, heredocs) => self.run_stages(commands, heredocs),
        }
    }

    /// Pipeline com mais de um comando: cada um num filho, ligados por
    /// `pipe2(O_CLOEXEC)` (POSIX 2.9.2). O status é o do último.
    fn run_stages(&mut self, commands: &[Command], heredocs: Vec<Vec<OwnedFd>>) -> i32 {
        let mut pids = Vec::with_capacity(commands.len());
        let mut prev_read: Option<OwnedFd> = None;
        let last = commands.len() - 1;
        let mut status = None;

        for (i, (cmd, fds)) in commands.iter().zip(heredocs).enumerate() {
            let pipe = if i < last {
                match nix::unistd::pipe2(OFlag::O_CLOEXEC) {
                    Ok(p) => Some(p),
                    Err(e) => {
                        self.error(format_args!("pipe: {}", e.desc()));
                        status = Some(1);
                        break;
                    }
                }
            } else {
                None
            };
            let _ = std::io::stdout().flush();
            match sys::fork() {
                Ok(sys::Fork::Child) => {
                    sys::reset_signals_in_child();
                    if let Some(read) = &prev_read {
                        let _ = sys::dup2_std(read.as_fd(), 0);
                    }
                    if let Some((_, write)) = &pipe {
                        let _ = sys::dup2_std(write.as_fd(), 1);
                    }
                    // Os originais têm O_CLOEXEC; fechamos já para um builtin
                    // (que não faz execve) não segurar a ponta de escrita.
                    drop(prev_read);
                    drop(pipe);
                    let code = match cmd {
                        Command::Simple(cmd) => self.run_simple(cmd, fds, Place::Child),
                        Command::Subshell { .. } => {
                            self.error("subshells: not supported");
                            STATUS_SYNTAX
                        }
                    };
                    let _ = std::io::stdout().flush();
                    sys::exit_child(code)
                }
                Ok(sys::Fork::Parent { child }) => {
                    pids.push(child);
                    // O pai fecha o que não usa: a ponta de leitura anterior e
                    // a de escrita nova (o drop do OwnedFd faz isso).
                    prev_read = pipe.map(|(read, _write)| read);
                }
                Err(e) => {
                    self.error(format_args!("fork: {}", e.desc()));
                    status = Some(1);
                    break;
                }
            }
        }
        drop(prev_read);

        let mut last_waited = None;
        for pid in pids {
            last_waited = Some(sys::wait(pid));
        }
        match (status, last_waited) {
            (Some(s), _) => s,
            (None, Some(waited)) => {
                self.report_signal(waited);
                waited.status()
            }
            (None, None) => 0,
        }
    }

    /// Comando simples (POSIX 2.9.1): expansão das palavras, depois os
    /// redirecionamentos, depois as atribuições; então builtin ou programa.
    fn run_simple(&mut self, cmd: &SimpleCommand, heredocs: Vec<OwnedFd>, place: Place) -> i32 {
        let (assignments, words) = split_assignments(&cmd.argv);
        let fields = expand_words(words, self);
        let mut heredocs = heredocs.into_iter();

        let Some(name) = fields.first() else {
            // Sem comando: os redirecionamentos são feitos (e desfeitos) e as
            // atribuições ficam no shell.
            let mut saved = redirect::Saved::default();
            if let Err(e) = redirect::apply(&cmd.redirects, &mut heredocs, self, Some(&mut saved)) {
                self.error(e);
                return 1;
            }
            drop(saved);
            for (name, value) in assignments {
                let value = expand_assignment(&value, self);
                self.env.set(name, value);
            }
            return 0;
        };

        if let Some(builtin) = Builtin::lookup(name) {
            // Builtin roda no processo atual: o shell, quando está sozinho,
            // ou o filho da pipeline.
            let mut saved = redirect::Saved::default();
            let saved_ref = (place == Place::Shell).then_some(&mut saved);
            if let Err(e) = redirect::apply(&cmd.redirects, &mut heredocs, self, saved_ref) {
                self.error(e);
                if builtin.is_special() && !self.interactive {
                    self.exit_request.get_or_insert(1);
                }
                return 1;
            }
            let assignments = assignments
                .into_iter()
                .map(|(name, value)| (name, expand_assignment(&value, self)))
                .collect();
            return self.run_builtin(builtin, &fields[1..], assignments);
        }

        match place {
            Place::Child => {
                if let Err(e) = redirect::apply(&cmd.redirects, &mut heredocs, self, None) {
                    self.error(e);
                    return 1;
                }
                let assignments: Vec<_> = assignments
                    .into_iter()
                    .map(|(name, value)| (name, expand_assignment(&value, self)))
                    .collect();
                self.exec_external(&fields, &assignments)
            }
            Place::Shell => {
                let assignments: Vec<_> = assignments
                    .into_iter()
                    .map(|(name, value)| (name, expand_assignment(&value, self)))
                    .collect();
                let _ = std::io::stdout().flush();
                match sys::fork() {
                    Ok(sys::Fork::Child) => {
                        sys::reset_signals_in_child();
                        if let Err(e) = redirect::apply(&cmd.redirects, &mut heredocs, self, None) {
                            self.error(e);
                            sys::exit_child(1);
                        }
                        let code = self.exec_external(&fields, &assignments);
                        sys::exit_child(code)
                    }
                    Ok(sys::Fork::Parent { child }) => {
                        drop(heredocs);
                        let waited = sys::wait(child);
                        self.report_signal(waited);
                        waited.status()
                    }
                    Err(e) => {
                        self.error(format_args!("fork: {}", e.desc()));
                        1
                    }
                }
            }
        }
    }

    /// Builtin no processo atual, com os redirecionamentos já aplicados.
    ///
    /// Atribuições antes de um builtin especial ficam no shell; antes de um
    /// builtin comum, valem só durante ele (POSIX 2.9.1).
    fn run_builtin(
        &mut self,
        builtin: Builtin,
        args: &[String],
        assignments: Vec<(String, String)>,
    ) -> i32 {
        let mut saved = Vec::new();
        for (name, value) in assignments {
            if !builtin.is_special() {
                saved.push((name.clone(), self.env.var(&name).cloned()));
                self.env.export(&name, None);
            }
            self.env.set(name, value);
        }

        let stdout = std::io::stdout();
        let stderr = std::io::stderr();
        let (mut out, mut err) = (stdout.lock(), stderr.lock());
        let status = builtin.run(
            args,
            &mut Io {
                out: &mut out,
                err: &mut err,
            },
            self,
        );

        for (name, var) in saved.into_iter().rev() {
            self.env.restore(&name, var);
        }
        if builtin.is_special() && status != 0 && !self.interactive {
            self.exit_request.get_or_insert(status);
        }
        status
    }

    /// No filho: resolve o nome e chama `execve`. Só retorna o status de
    /// erro (126 ou 127), com a mensagem já escrita.
    fn exec_external(&self, argv: &[String], assignments: &[(String, String)]) -> i32 {
        let path = match path::resolve(&argv[0], self) {
            Ok(path) => path,
            Err(e) => {
                self.error(e);
                return STATUS_NOT_FOUND;
            }
        };
        let envp = if assignments.is_empty() {
            self.env.to_envp()
        } else {
            let mut env = self.env.clone();
            for (name, value) in assignments {
                env.export(name.clone(), Some(value.clone()));
            }
            env.to_envp()
        };
        let Ok(c_path) = CString::new(path.as_os_str().as_bytes()) else {
            self.error(format_args!("{}: invalid name", argv[0]));
            return STATUS_NOT_FOUND;
        };
        let c_argv: Vec<CString> = argv.iter().map(|a| to_cstring(a)).collect();
        let errno = sys::execve(&c_path, &c_argv, &envp);
        self.report_exec_error(&argv[0], &path, errno, &c_argv, &envp)
    }

    /// Mensagem quando o comando morre por sinal, como no bash: quebra de
    /// linha depois do `^C` no terminal, e `Quit (core dumped)` para Ctrl+\\.
    fn report_signal(&self, waited: sys::Waited) {
        use nix::sys::signal::Signal;
        let sys::Waited::Signaled(sig, core) = waited else {
            return;
        };
        let core = if core { " (core dumped)" } else { "" };
        match sig {
            Signal::SIGINT if self.interactive => eprintln!(),
            Signal::SIGQUIT => eprintln!("Quit{core}"),
            Signal::SIGKILL => eprintln!("Killed{core}"),
            Signal::SIGTERM => eprintln!("Terminated{core}"),
            Signal::SIGSEGV => eprintln!("Segmentation fault{core}"),
            Signal::SIGABRT => eprintln!("Aborted{core}"),
            _ => {}
        }
    }

    /// No filho, depois de um `execve` que falhou: mensagem e status do
    /// POSIX 2.9.1.6 (126 achado mas não executável, 127 não existe).
    fn report_exec_error(
        &self,
        name: &str,
        path: &std::path::Path,
        errno: Errno,
        argv: &[CString],
        envp: &[CString],
    ) -> i32 {
        match errno {
            // Arquivo sem cabeçalho reconhecido: o POSIX manda executá-lo como
            // script do próprio shell.
            Errno::ENOEXEC => {
                if let Ok(exe) = std::env::current_exe()
                    && let Ok(c_exe) = CString::new(exe.as_os_str().as_bytes())
                {
                    let mut script_argv = vec![to_cstring(&self.name)];
                    script_argv.push(to_cstring(&path.to_string_lossy()));
                    script_argv.extend(argv.iter().skip(1).cloned());
                    let _ = sys::execve(&c_exe, &script_argv, envp);
                }
                self.error(format_args!("{name}: {}", Errno::ENOEXEC.desc()));
                STATUS_NOT_EXECUTABLE
            }
            Errno::ENOENT | Errno::ENOTDIR => {
                self.error(format_args!("{name}: {}", errno.desc()));
                STATUS_NOT_FOUND
            }
            Errno::EACCES if path.is_dir() => {
                self.error(format_args!("{name}: Is a directory"));
                STATUS_NOT_EXECUTABLE
            }
            _ => {
                self.error(format_args!("{name}: {}", errno.desc()));
                STATUS_NOT_EXECUTABLE
            }
        }
    }
}

/// Converte para `CString`, cortando no primeiro NUL (que o lexer nunca
/// produz, mas pode vir do terminal).
fn to_cstring(s: &str) -> CString {
    let bytes = s.as_bytes();
    let end = bytes.iter().position(|&b| b == 0).unwrap_or(bytes.len());
    CString::new(&bytes[..end]).unwrap_or_default()
}

/// Separa as atribuições do início do comando (`A=1 B=2 cmd`), POSIX 2.9.1.
///
/// Uma palavra é atribuição quando começa, sem aspas, com um nome válido
/// seguido de `=`. O valor vira uma nova [`Word`] com o resto das partes.
fn split_assignments(argv: &[Word]) -> (Vec<(String, Word)>, &[Word]) {
    let mut assignments = Vec::new();
    for (i, word) in argv.iter().enumerate() {
        let Some(WordPart::Literal(first)) = word.parts.first() else {
            return (assignments, &argv[i..]);
        };
        let Some((name, rest)) = first.split_once('=') else {
            return (assignments, &argv[i..]);
        };
        if !is_valid_name(name) {
            return (assignments, &argv[i..]);
        }
        let mut parts = Vec::with_capacity(word.parts.len());
        if !rest.is_empty() {
            parts.push(WordPart::Literal(rest.to_owned()));
        }
        parts.extend(word.parts[1..].iter().cloned());
        assignments.push((name.to_owned(), Word { parts }));
    }
    (assignments, &[])
}
