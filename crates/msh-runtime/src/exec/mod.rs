//! Execução (POSIX 2.9): da linha de texto ao status de saída.
//!
//! [`Shell::execute_line`] é a fachada (o `facade.c` da versão em C):
//! analisa, expande e executa, e grava o `$?`.

pub mod path;
pub mod sys;

use std::ffi::CString;
use std::io::Write;
use std::ops::ControlFlow;
use std::os::unix::ffi::OsStrExt;

use msh_syntax::{Command, Node, SimpleCommand, Word, WordPart};
use nix::errno::Errno;

use crate::builtins::{Builtin, Io};
use crate::env::is_valid_name;
use crate::expand::{expand_assignment, expand_words};
use crate::shell::Shell;

/// Status de erro de sintaxe e de uso (POSIX 2.8.2).
pub const STATUS_SYNTAX: i32 = 2;
/// Comando achado, mas não executável (POSIX 2.8.2).
pub const STATUS_NOT_EXECUTABLE: i32 = 126;
/// Comando não encontrado (POSIX 2.8.2).
pub const STATUS_NOT_FOUND: i32 = 127;

impl Shell {
    /// Analisa e executa uma linha.
    ///
    /// Retorna `Break(código)` quando o shell deve sair: `exit`, ou um erro
    /// que encerra o shell não interativo (erro de sintaxe ou de builtin
    /// especial, POSIX 2.8.1).
    pub fn execute_line(&mut self, line: &str) -> ControlFlow<i32> {
        match msh_syntax::parse(line) {
            Ok(None) => {}
            Ok(Some(node)) => self.last_status = self.execute(&node),
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
    pub fn execute(&mut self, node: &Node) -> i32 {
        match node {
            Node::Pipeline(pipeline) => match pipeline.commands.as_slice() {
                [Command::Simple(cmd)] if cmd.redirects.is_empty() => self.run_simple(cmd),
                [Command::Simple(_)] => self.not_yet("redirections"),
                [_] => self.not_yet("subshells"),
                _ => self.not_yet("pipelines"),
            },
            Node::And(..) | Node::Or(..) => self.not_yet("&& and ||"),
        }
    }

    // M4 tira isto: pipes e redirecionamentos.
    fn not_yet(&self, what: &str) -> i32 {
        self.error(format_args!("{what}: not implemented yet"));
        STATUS_SYNTAX
    }

    /// Comando simples (POSIX 2.9.1): atribuições, expansão, e então
    /// builtin ou programa externo.
    fn run_simple(&mut self, cmd: &SimpleCommand) -> i32 {
        let (assignments, words) = split_assignments(&cmd.argv);
        let fields = expand_words(words, self);
        let assignments: Vec<(String, String)> = assignments
            .into_iter()
            .map(|(name, value)| (name, expand_assignment(&value, self)))
            .collect();

        let Some(name) = fields.first() else {
            // Só atribuições: valem no shell.
            for (name, value) in assignments {
                self.env.set(name, value);
            }
            return 0;
        };
        match Builtin::lookup(name) {
            Some(builtin) => self.run_builtin(builtin, &fields[1..], assignments),
            None => self.run_external(&fields, &assignments),
        }
    }

    /// Builtin no próprio processo do shell.
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

    /// Programa externo: `fork`, `execve` no filho e `waitpid` no pai.
    fn run_external(&mut self, argv: &[String], assignments: &[(String, String)]) -> i32 {
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
        // Tudo o que o filho precisa é preparado antes do fork.
        let Ok(c_path) = CString::new(path.as_os_str().as_bytes()) else {
            self.error(format_args!("{}: invalid name", argv[0]));
            return STATUS_NOT_FOUND;
        };
        let c_argv: Vec<CString> = argv.iter().map(|a| to_cstring(a)).collect();
        // O filho herda os buffers do pai: esvazia antes.
        let _ = std::io::stdout().flush();

        match sys::fork() {
            Ok(sys::Fork::Child) => {
                sys::reset_signals_in_child();
                let errno = sys::execve(&c_path, &c_argv, &envp);
                let code = self.report_exec_error(&argv[0], &path, errno, &c_argv, &envp);
                sys::exit_child(code)
            }
            Ok(sys::Fork::Parent { child }) => {
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
