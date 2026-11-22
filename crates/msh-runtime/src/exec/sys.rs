//! Único módulo com `unsafe` do workspace (ADR-005).
//!
//! Tudo o que precisa de `unsafe` passa por aqui, com o motivo num
//! comentário `// SAFETY:`. O resto do runtime só vê funções seguras.
#![allow(unsafe_code)]

use std::ffi::{CStr, CString};
use std::os::fd::{BorrowedFd, OwnedFd};

use nix::errno::Errno;
use nix::sys::signal::{SigHandler, Signal, signal};
use nix::sys::wait::{WaitStatus, waitpid};
use nix::unistd::{ForkResult, Pid};

pub use nix::unistd::ForkResult as Fork;

/// `fork()`.
///
/// # Errors
///
/// Erro do kernel (por exemplo, `EAGAIN` sem processos livres).
pub fn fork() -> nix::Result<ForkResult> {
    // SAFETY: o minishell tem uma thread só (a GUI roda o shell como processo
    // separado, ADR-011), então o filho herda um estado consistente e pode
    // alocar memória e rodar builtins. Os testes de integração rodam o
    // binário, e não o runtime dentro do harness multithread do cargo test.
    unsafe { nix::unistd::fork() }
}

/// Restaura a disposição padrão dos sinais no filho, antes do `execve` ou de
/// rodar um builtin numa pipeline.
///
/// O Rust ignora `SIGPIPE` na partida, e o shell interativo ignora `SIGINT` e
/// `SIGQUIT` enquanto espera (ADR-008); disposições ignoradas sobrevivem ao
/// `execve`, então precisam voltar para `SIG_DFL` aqui.
pub fn reset_signals_in_child() {
    for sig in [Signal::SIGINT, Signal::SIGQUIT, Signal::SIGPIPE] {
        set_handler(sig, SigHandler::SigDfl);
    }
}

/// Ignora `SIGINT` e `SIGQUIT` no pai enquanto um comando roda.
pub fn ignore_job_signals() {
    for sig in [Signal::SIGINT, Signal::SIGQUIT] {
        set_handler(sig, SigHandler::SigIgn);
    }
}

fn set_handler(sig: Signal, handler: SigHandler) {
    // SAFETY: só usamos SIG_DFL e SIG_IGN, que não executam código nosso
    // dentro de um handler; não há estado compartilhado com sinais.
    let _ = unsafe { signal(sig, handler) };
}

/// `execve`. Só retorna em caso de erro.
#[must_use]
pub fn execve(path: &CStr, argv: &[CString], envp: &[CString]) -> Errno {
    match nix::unistd::execve(path, argv, envp) {
        Err(e) => e,
        Ok(never) => match never {},
    }
}

/// Termina o processo filho sem rodar destrutores nem esvaziar buffers que
/// foram copiados do pai.
pub fn exit_child(code: i32) -> ! {
    // SAFETY: `_exit` só encerra o processo; não toca em memória do Rust.
    unsafe { libc::_exit(code) }
}

/// Espera um filho e devolve o status como o `$?` do POSIX (2.8.2):
/// `Exited(c)` vira `c`, `Signaled(s)` vira `128 + s`.
#[must_use]
pub fn wait(pid: Pid) -> Waited {
    loop {
        match waitpid(pid, None) {
            Ok(WaitStatus::Exited(_, code)) => return Waited::Exited(code),
            Ok(WaitStatus::Signaled(_, sig, core)) => return Waited::Signaled(sig, core),
            Err(Errno::EINTR) | Ok(_) => {}
            Err(_) => return Waited::Exited(1),
        }
    }
}

/// Como um filho terminou.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Waited {
    Exited(i32),
    /// Sinal e se houve core dump.
    Signaled(Signal, bool),
}

impl Waited {
    #[must_use]
    pub fn status(self) -> i32 {
        match self {
            Self::Exited(code) => code,
            Self::Signaled(sig, _) => 128 + sig as i32,
        }
    }
}

/// Aponta o fd padrão `target` (0, 1 ou 2) para `fd`.
///
/// # Errors
///
/// Erro do kernel.
pub fn dup2_std(fd: BorrowedFd<'_>, target: i32) -> nix::Result<()> {
    match target {
        0 => nix::unistd::dup2_stdin(fd),
        1 => nix::unistd::dup2_stdout(fd),
        _ => nix::unistd::dup2_stderr(fd),
    }
}

/// Cópia de `fd` com `O_CLOEXEC`, acima do 9 (para não ocupar fds que o
/// usuário pode citar).
///
/// # Errors
///
/// Erro do kernel.
pub fn dup_cloexec(fd: BorrowedFd<'_>) -> nix::Result<OwnedFd> {
    let raw = nix::fcntl::fcntl(fd, nix::fcntl::FcntlArg::F_DUPFD_CLOEXEC(10))?;
    // SAFETY: o fcntl acabou de criar este fd, que não pertence a mais
    // ninguém.
    Ok(unsafe { std::os::fd::FromRawFd::from_raw_fd(raw) })
}
