//! Terminal embutido (RF-21): um `vte::Terminal` rodando o `minishell
//! --osc7` num PTY, e o caminho shell → tela (RF-22): a cada prompt o shell
//! manda OSC 7, o VTE dispara `current-directory-uri-changed` e a janela
//! vai para essa pasta.

use std::path::{Path, PathBuf};

use gtk::glib;
use gtk::prelude::*;
use vte::prelude::*;

use crate::window::{Origin, Window};

pub fn new_terminal() -> vte::Terminal {
    let t = vte::Terminal::builder()
        .scrollback_lines(10_000)
        .hexpand(true)
        .vexpand(true)
        .build();
    t.set_size_request(-1, 160);
    t
}

/// Onde está o `minishell`: `$MIAU_FILES_SHELL`, ao lado do `miau-files`
/// (o caso de `target/debug/` e de `/usr/bin/`), ou no `PATH`.
pub fn shell_program() -> PathBuf {
    if let Some(p) = std::env::var_os("MIAU_FILES_SHELL") {
        return PathBuf::from(p);
    }
    let sibling = std::env::current_exe()
        .ok()
        .and_then(|exe| exe.parent().map(|d| d.join("minishell")));
    match sibling {
        Some(p) if p.is_file() => p,
        _ => glib::find_program_in_path("minishell").unwrap_or_else(|| "minishell".into()),
    }
}

/// Abre o minishell no terminal, na pasta `dir`. Quando ele termina (por
/// exemplo, com `exit`), abre outro na pasta da tela.
pub fn spawn_shell(win: &Window, dir: &Path) {
    let program = shell_program();
    let program = program.to_string_lossy();
    let dir = dir.to_string_lossy();
    let w = win.clone();
    win.terminal.spawn_async(
        vte::PtyFlags::DEFAULT,
        Some(&dir),
        &[&program, "--osc7"],
        &[],
        glib::SpawnFlags::SEARCH_PATH,
        || {},
        -1,
        None::<&gtk::gio::Cancellable>,
        move |res| match res {
            Ok(pid) => {
                w.shell_pid.set(u32::try_from(pid.0).ok());
                w.shell_dir.replace(None);
            }
            Err(e) => w.toast(&format!("Não deu para abrir o minishell: {}", e.message())),
        },
    );
}

/// Liga os sinais do terminal à janela.
pub fn connect(win: &Window) {
    let w = win.clone();
    #[allow(deprecated)] // VTE 0.78+ prefere termprops; o Debian 13 tem as duas.
    win.terminal
        .connect_current_directory_uri_changed(move |t| {
            #[allow(deprecated)]
            let Some(uri) = t.current_directory_uri() else {
                return;
            };
            let Some(path) = msh_files::uri::file_uri_path(&uri) else {
                return;
            };
            // O shell manda OSC 7 a cada prompt, mesmo sem `cd`. Só uma pasta
            // nova move a tela; assim, uma navegação feita com o terminal
            // ocupado não é desfeita no próximo prompt.
            if w.shell_dir.borrow().as_deref() == Some(path.as_path()) {
                return;
            }
            w.shell_dir.replace(Some(path.clone()));
            w.navigate(&path, Origin::Shell);
        });

    let w = win.clone();
    win.terminal.connect_child_exited(move |_, _status| {
        w.shell_pid.set(None);
        let dir = w.current_dir();
        spawn_shell(&w, &dir);
    });
}
