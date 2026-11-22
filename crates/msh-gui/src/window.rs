//! A janela: barra de caminho, barra lateral, lista de arquivos e terminal,
//! e a navegação que mantém a tela e o shell na mesma pasta (RF-20, RF-22).

use std::cell::{Cell, RefCell};
use std::path::{Path, PathBuf};
use std::rc::Rc;

use adw::prelude::*;
use gtk::{gio, glib};
use msh_files::history::History;
use vte::prelude::*;

use crate::{files, sidebar, terminal};

/// Quem pediu a navegação. Só a tela e o histórico mandam `cd` ao shell; o
/// que vem do shell (OSC 7) só muda a tela.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Origin {
    Screen,
    History,
    Shell,
}

/// O que aconteceu com o shell numa navegação (usado pelo autoteste).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ShellSync {
    /// O `cd` foi digitado no terminal.
    Sent,
    /// O shell estava rodando um programa; o terminal ficou onde estava.
    Busy,
    /// Nada a fazer (a navegação veio do shell, ou ele ainda não abriu).
    None,
}

#[derive(Debug)]
pub struct Inner {
    pub window: adw::ApplicationWindow,
    pub toasts: adw::ToastOverlay,
    pub path_entry: gtk::Entry,
    pub back: gtk::Button,
    pub forward: gtk::Button,
    pub up: gtk::Button,
    pub list: files::FileList,
    pub terminal: vte::Terminal,
    pub sidebar: sidebar::Sidebar,
    pub history: RefCell<History>,
    /// Pid do minishell no terminal, quando ele já abriu.
    pub shell_pid: Cell<Option<u32>>,
    /// A última pasta que o shell anunciou por OSC 7.
    pub shell_dir: RefCell<Option<PathBuf>>,
    pub last_sync: Cell<ShellSync>,
}

/// Uma referência barata para a janela e o seu estado.
#[derive(Debug, Clone)]
pub struct Window(pub Rc<Inner>);

impl std::ops::Deref for Window {
    type Target = Inner;
    fn deref(&self) -> &Inner {
        &self.0
    }
}

impl Window {
    pub fn new(app: &adw::Application, start: &Path) -> Self {
        let back = icon_button("go-previous-symbolic", "Voltar (Alt+←)");
        let forward = icon_button("go-next-symbolic", "Avançar (Alt+→)");
        let up = icon_button("go-up-symbolic", "Pasta acima (Alt+↑)");
        let path_entry = gtk::Entry::builder()
            .hexpand(true)
            .placeholder_text("Caminho")
            .build();

        let header = adw::HeaderBar::new();
        header.pack_start(&back);
        header.pack_start(&forward);
        header.pack_start(&up);
        header.set_title_widget(Some(&path_entry));

        let list = files::FileList::new();
        let sidebar = sidebar::Sidebar::new();
        let terminal = terminal::new_terminal();

        let terminal_box = gtk::ScrolledWindow::builder()
            .child(&terminal)
            .vexpand(true)
            .build();
        let right = gtk::Paned::builder()
            .orientation(gtk::Orientation::Vertical)
            .start_child(&list.scrolled)
            .end_child(&terminal_box)
            .resize_start_child(true)
            .shrink_end_child(false)
            .position(380)
            .build();
        let main = gtk::Paned::builder()
            .orientation(gtk::Orientation::Horizontal)
            .start_child(&sidebar.scrolled)
            .end_child(&right)
            .resize_start_child(false)
            .shrink_start_child(false)
            .position(200)
            .build();
        let toasts = adw::ToastOverlay::new();
        toasts.set_child(Some(&main));

        let view = adw::ToolbarView::new();
        view.add_top_bar(&header);
        view.set_content(Some(&toasts));

        let window = adw::ApplicationWindow::builder()
            .application(app)
            .title("Miau Files")
            .default_width(1000)
            .default_height(720)
            .content(&view)
            .build();

        let win = Self(Rc::new(Inner {
            window,
            toasts,
            path_entry,
            back,
            forward,
            up,
            list,
            terminal,
            sidebar,
            history: RefCell::new(History::new(start.to_path_buf())),
            shell_pid: Cell::new(None),
            shell_dir: RefCell::new(None),
            last_sync: Cell::new(ShellSync::None),
        }));
        win.connect_signals();
        win.show_dir(start);
        terminal::spawn_shell(&win, start);
        win
    }

    pub fn present(&self) {
        self.window.present();
    }

    pub fn current_dir(&self) -> PathBuf {
        self.history.borrow().current().to_path_buf()
    }

    /// Vai para `path`. Da tela ou do histórico, também leva o shell junto.
    pub fn navigate(&self, path: &Path, origin: Origin) {
        let path = clean(path);
        match std::fs::metadata(&path) {
            Ok(m) if m.is_dir() => {}
            Ok(_) => return self.toast(&format!("“{}” não é uma pasta", path.display())),
            Err(e) => {
                return self.toast(&format!(
                    "Não deu para abrir “{}”: {}",
                    path.display(),
                    msh_files::format::io_error(&e)
                ));
            }
        }
        if origin != Origin::History && !self.history.borrow_mut().visit(path.clone()) {
            // Já estamos lá (por exemplo, o OSC 7 que segue o nosso `cd`).
            self.last_sync.set(ShellSync::None);
            return;
        }
        self.show_dir(&path);
        let sync = if origin == Origin::Shell {
            ShellSync::None
        } else {
            self.sync_shell(&path)
        };
        self.last_sync.set(sync);
    }

    pub fn go_back(&self) {
        let prev = self.history.borrow_mut().go_back().map(Path::to_path_buf);
        if let Some(p) = prev {
            self.navigate(&p, Origin::History);
        }
    }

    pub fn go_forward(&self) {
        let next = self
            .history
            .borrow_mut()
            .go_forward()
            .map(Path::to_path_buf);
        if let Some(p) = next {
            self.navigate(&p, Origin::History);
        }
    }

    pub fn go_up(&self) {
        let cur = self.current_dir();
        self.navigate(&msh_files::paths::parent_or_root(&cur), Origin::Screen);
    }

    pub fn toast(&self, msg: &str) {
        self.toasts.add_toast(adw::Toast::new(msg));
    }

    /// Mostra `path` na tela, sem mexer no histórico nem no shell.
    fn show_dir(&self, path: &Path) {
        self.list.set_dir(path);
        self.path_entry.set_text(&path.to_string_lossy());
        self.window
            .set_title(Some(&format!("{} — Miau Files", display_name(path))));
        let h = self.history.borrow();
        self.back.set_sensitive(h.can_go_back());
        self.forward.set_sensitive(h.can_go_forward());
        self.up.set_sensitive(path != Path::new("/"));
        self.sidebar.select(path);
    }

    /// Tela → shell: digita `cd '<pasta>'` no terminal, se o shell estiver
    /// parado no prompt (TDD §7).
    fn sync_shell(&self, path: &Path) -> ShellSync {
        let Some(pid) = self.shell_pid.get() else {
            return ShellSync::None;
        };
        if self.shell_dir.borrow().as_deref() == Some(path) {
            return ShellSync::None;
        }
        if msh_files::proc::has_children(Path::new("/proc"), pid) {
            self.toast("O terminal está ocupado; ele continua na pasta anterior");
            return ShellSync::Busy;
        }
        // Ctrl+E e Ctrl+U apagam o que estiver digitado no prompt, para o
        // `cd` não ser grudado num comando pela metade.
        let mut text = b"\x05\x15".to_vec();
        text.extend_from_slice(msh_files::paths::cd_command(path).as_bytes());
        self.terminal.feed_child(&text);
        ShellSync::Sent
    }

    fn connect_signals(&self) {
        let w = self.clone();
        self.back.connect_clicked(move |_| w.go_back());
        let w = self.clone();
        self.forward.connect_clicked(move |_| w.go_forward());
        let w = self.clone();
        self.up.connect_clicked(move |_| w.go_up());

        let w = self.clone();
        self.path_entry.connect_activate(move |entry| {
            let text = entry.text();
            let path = expand_tilde(text.trim());
            if path.is_absolute() {
                w.navigate(&path, Origin::Screen);
            } else {
                w.navigate(&w.current_dir().join(path), Origin::Screen);
            }
        });

        let w = self.clone();
        self.list.view.connect_activate(move |_, pos| {
            if let Some(info) = w.list.info_at(pos) {
                let path = w.current_dir().join(info.name());
                if info.file_type() == gio::FileType::Directory {
                    w.navigate(&path, Origin::Screen);
                }
            }
        });

        let w = self.clone();
        self.list.dir_list.connect_error_notify(move |list| {
            if let Some(err) = list.error() {
                w.toast(&format!("Não deu para listar a pasta: {}", err.message()));
            }
        });

        let w = self.clone();
        self.sidebar.list.connect_row_activated(move |_, row| {
            if let Some(path) = w.sidebar.path_of(row) {
                w.navigate(&path, Origin::Screen);
            }
        });

        terminal::connect(self);
        self.add_shortcuts();
    }

    fn add_shortcuts(&self) {
        let actions: [Shortcut; 5] = [
            ("back", &["<Alt>Left"], Self::go_back),
            ("forward", &["<Alt>Right"], Self::go_forward),
            ("up", &["<Alt>Up"], Self::go_up),
            ("location", &["<Control>l"], |w| {
                w.path_entry.grab_focus();
            }),
            ("hidden", &["<Control>h"], |w| w.list.toggle_hidden()),
        ];
        let app = self.window.application();
        for (name, accels, f) in actions {
            let action = gio::SimpleAction::new(name, None);
            let w = self.clone();
            action.connect_activate(move |_, _| f(&w));
            self.window.add_action(&action);
            if let Some(app) = &app {
                app.set_accels_for_action(&format!("win.{name}"), accels);
            }
        }
    }
}

/// Nome da ação, atalhos e o que ela faz.
type Shortcut = (&'static str, &'static [&'static str], fn(&Window));

fn icon_button(icon: &str, tooltip: &str) -> gtk::Button {
    gtk::Button::builder()
        .icon_name(icon)
        .tooltip_text(tooltip)
        .build()
}

/// Remove `.`, `..` e barras repetidas sem resolver links, como o `cd -L`.
fn clean(path: &Path) -> PathBuf {
    let mut out = PathBuf::from("/");
    for c in path.components() {
        match c {
            std::path::Component::ParentDir => {
                out.pop();
            }
            std::path::Component::Normal(n) => out.push(n),
            _ => {}
        }
    }
    out
}

fn expand_tilde(text: &str) -> PathBuf {
    let home = glib::home_dir();
    if text == "~" {
        home
    } else if let Some(rest) = text.strip_prefix("~/") {
        home.join(rest)
    } else {
        PathBuf::from(text)
    }
}

pub fn display_name(path: &Path) -> String {
    path.file_name()
        .map_or_else(|| "/".to_owned(), |n| n.to_string_lossy().into_owned())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn cleans_like_cd_l() {
        assert_eq!(clean(Path::new("/a/./b/../c//d")), PathBuf::from("/a/c/d"));
        assert_eq!(clean(Path::new("/..")), PathBuf::from("/"));
        assert_eq!(display_name(Path::new("/")), "/");
        assert_eq!(display_name(Path::new("/etc/apt")), "apt");
    }
}
