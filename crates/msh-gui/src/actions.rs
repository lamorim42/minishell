//! Ações sobre arquivos (RF-23): abrir, copiar, recortar, colar, renomear,
//! nova pasta, lixeira e favoritos. Quando uma delas esbarra em "permissão
//! negada", a GUI oferece refazer como administrador (RF-24, `admin.rs`).
//!
//! Os atalhos de teclado valem só com o foco na lista de arquivos: no
//! terminal, Ctrl+C, Ctrl+D e Ctrl+H continuam indo para o shell.

use std::io;
use std::path::{Path, PathBuf};

use adw::prelude::*;
use gtk::{gio, glib};
use msh_files::helper::Action;
use msh_files::ops;

use crate::admin;
use crate::window::{Origin, Window};

/// Copiar ou recortar.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ClipMode {
    Copy,
    Cut,
}

/// A área de transferência interna da janela.
#[derive(Debug, Clone)]
pub struct Clip {
    pub mode: ClipMode,
    pub paths: Vec<PathBuf>,
}

type Handler = fn(&Window);

/// Nome da ação, atalho (só na lista), rótulo no menu de contexto.
const LIST_ACTIONS: [(&str, &str, &str, Handler); 9] = [
    ("open", "", "Abrir", |w| {
        for p in selected_paths(w) {
            open(w, &p);
        }
    }),
    ("copy", "<Control>c", "Copiar", |w| {
        set_clip(w, ClipMode::Copy);
    }),
    ("cut", "<Control>x", "Recortar", |w| {
        set_clip(w, ClipMode::Cut);
    }),
    ("paste", "<Control>v", "Colar", |w| {
        let w = w.clone();
        glib::spawn_future_local(async move {
            let dir = w.current_dir();
            paste(&w, &dir).await;
        });
    }),
    ("rename", "F2", "Renomear…", |w| {
        let w = w.clone();
        glib::spawn_future_local(async move { ask_rename(&w).await });
    }),
    ("new-folder", "<Control><Shift>n", "Nova pasta…", |w| {
        let w = w.clone();
        glib::spawn_future_local(async move { ask_new_folder(&w).await });
    }),
    ("trash", "Delete", "Mover para a lixeira", trash),
    (
        "bookmark",
        "<Control>d",
        "Favoritar esta pasta",
        toggle_bookmark,
    ),
    ("edit-admin", "", "Editar como administrador…", |w| {
        if let [path] = selected_paths(w).as_slice() {
            admin::edit(w, path);
        } else {
            w.toast("Selecione um arquivo para editar");
        }
    }),
];

/// Cria as ações `win.*`, os atalhos da lista e o menu de contexto.
pub fn install(win: &Window) {
    let shortcuts = gtk::ShortcutController::new();
    shortcuts.set_scope(gtk::ShortcutScope::Local);
    let menu = gio::Menu::new();
    for (name, accel, label, f) in LIST_ACTIONS {
        let action = gio::SimpleAction::new(name, None);
        let w = win.clone();
        action.connect_activate(move |_, _| f(&w));
        win.window.add_action(&action);
        if !accel.is_empty() {
            shortcuts.add_shortcut(gtk::Shortcut::new(
                gtk::ShortcutTrigger::parse_string(accel),
                Some(gtk::NamedAction::new(&format!("win.{name}"))),
            ));
        }
        if name != "bookmark" {
            menu.append(Some(label), Some(&format!("win.{name}")));
        }
    }
    win.list.view.add_controller(shortcuts);

    // Menu de contexto (botão direito) na lista.
    let popover = gtk::PopoverMenu::from_model(Some(&menu));
    popover.set_parent(&win.list.view);
    popover.set_has_arrow(false);
    popover.set_halign(gtk::Align::Start);
    let click = gtk::GestureClick::builder().button(3).build();
    click.connect_pressed(move |_, _, x, y| {
        #[allow(clippy::cast_possible_truncation)] // coordenadas da tela
        popover.set_pointing_to(Some(&gtk::gdk::Rectangle::new(x as i32, y as i32, 1, 1)));
        popover.popup();
    });
    win.list.view.add_controller(click);

    let w = win.clone();
    win.star.connect_clicked(move |_| toggle_bookmark(&w));
    win.reload_bookmarks();
}

/// Os caminhos selecionados na lista.
pub fn selected_paths(win: &Window) -> Vec<PathBuf> {
    let dir = win.current_dir();
    win.list
        .selected()
        .iter()
        .map(|i| dir.join(i.name()))
        .collect()
}

/// Abre `path`: pasta, navega (e leva o shell junto); arquivo, abre no
/// aplicativo padrão (GIO).
pub fn open(win: &Window, path: &Path) {
    if path.is_dir() {
        win.navigate(path, Origin::Screen);
        return;
    }
    let uri = gio::File::for_path(path).uri();
    if let Err(e) = gio::AppInfo::launch_default_for_uri(&uri, None::<&gio::AppLaunchContext>) {
        win.toast(&format!("Não deu para abrir: {}", e.message()));
    }
}

fn set_clip(win: &Window, mode: ClipMode) {
    let paths = selected_paths(win);
    if paths.is_empty() {
        return win.toast("Nada selecionado");
    }
    let n = paths.len();
    win.clipboard.replace(Some(Clip { mode, paths }));
    let verb = if mode == ClipMode::Copy {
        "copiado"
    } else {
        "recortado"
    };
    win.toast(&if n == 1 {
        format!("1 item {verb}")
    } else {
        format!("{n} itens {verb}s")
    });
}

/// Cola o que estiver na área de transferência em `dir`. Devolve os
/// caminhos criados. Um item sem permissão pode ser refeito como
/// administrador.
pub async fn paste(win: &Window, dir: &Path) -> Vec<PathBuf> {
    let Some(clip) = win.clipboard.borrow().clone() else {
        win.toast("A área de transferência está vazia");
        return Vec::new();
    };
    let mut done = Vec::new();
    for src in clip.paths.clone() {
        let dest = dir.to_path_buf();
        let src2 = src.clone();
        let result = blocking(move || match clip.mode {
            ClipMode::Copy => ops::copy_into(&src2, &dest),
            ClipMode::Cut => ops::move_into(&src2, &dest),
        })
        .await;
        match result {
            Ok(p) => done.push(p),
            Err(e) if e.kind() == io::ErrorKind::PermissionDenied => {
                let action = match clip.mode {
                    ClipMode::Copy => Action::Copy {
                        src: src.clone(),
                        dest_dir: dir.to_path_buf(),
                    },
                    ClipMode::Cut => Action::Move {
                        src: src.clone(),
                        dest_dir: dir.to_path_buf(),
                    },
                };
                let what = format!("colar “{}” aqui", file_name(&src));
                admin::retry(win, &what, action).await;
            }
            Err(e) => win.toast(&format!(
                "Não deu para colar “{}”: {}",
                file_name(&src),
                msh_files::format::io_error(&e)
            )),
        }
    }
    if clip.mode == ClipMode::Cut {
        win.clipboard.replace(None);
    }
    win.list.reload();
    done
}

/// Renomeia `path`; sem permissão, oferece o administrador.
pub async fn rename(win: &Window, path: &Path, new_name: &str) -> Option<PathBuf> {
    let (p, n) = (path.to_path_buf(), new_name.to_owned());
    match blocking(move || ops::rename(&p, &n)).await {
        Ok(dest) => {
            win.list.reload();
            Some(dest)
        }
        Err(e) if e.kind() == io::ErrorKind::PermissionDenied => {
            let what = format!("renomear “{}”", file_name(path));
            let action = Action::Rename {
                path: path.to_path_buf(),
                new_name: new_name.to_owned(),
            };
            admin::retry(win, &what, action).await;
            None
        }
        Err(e) => {
            win.toast(&format!(
                "Não deu para renomear: {}",
                msh_files::format::io_error(&e)
            ));
            None
        }
    }
}

async fn ask_rename(win: &Window) {
    let Ok([path]) = <[PathBuf; 1]>::try_from(selected_paths(win)) else {
        return win.toast("Selecione um item para renomear");
    };
    if let Some(name) = ask_name(win, "Renomear", &file_name(&path), "Renomear").await {
        rename(win, &path, &name).await;
    }
}

async fn ask_new_folder(win: &Window) {
    let Some(name) = ask_name(win, "Nova pasta", "", "Criar").await else {
        return;
    };
    if let Err(e) = msh_files::paths::check_name(&name) {
        return win.toast(&e.to_string());
    }
    let path = win.current_dir().join(&name);
    let p = path.clone();
    match blocking(move || ops::make_dir(&p)).await {
        Ok(()) => {}
        Err(e) if e.kind() == io::ErrorKind::PermissionDenied => {
            admin::retry(win, "criar uma pasta aqui", Action::MakeDir { path }).await;
        }
        Err(e) => win.toast(&format!(
            "Não deu para criar: {}",
            msh_files::format::io_error(&e)
        )),
    }
}

/// Pede um nome num diálogo. `None` se o usuário cancelar.
async fn ask_name(win: &Window, heading: &str, initial: &str, ok: &str) -> Option<String> {
    let entry = gtk::Entry::builder()
        .text(initial)
        .activates_default(true)
        .build();
    let dialog = adw::AlertDialog::new(Some(heading), None);
    dialog.set_extra_child(Some(&entry));
    dialog.add_response("cancel", "Cancelar");
    dialog.add_response("ok", ok);
    dialog.set_response_appearance("ok", adw::ResponseAppearance::Suggested);
    dialog.set_default_response(Some("ok"));
    dialog.set_close_response("cancel");
    // Seleciona o nome sem a extensão, como o GNOME Arquivos.
    let stem = initial
        .rfind('.')
        .filter(|&i| i > 0)
        .unwrap_or(initial.len());
    let stem = i32::try_from(initial[..stem].chars().count()).unwrap_or(-1);
    entry.connect_map(move |e| e.select_region(0, stem));
    let answer = dialog.choose_future(Some(&win.window)).await;
    let name = entry.text().trim().to_owned();
    (answer == "ok" && !name.is_empty()).then_some(name)
}

fn trash(win: &Window) {
    for path in selected_paths(win) {
        let w = win.clone();
        glib::spawn_future_local(async move {
            let file = gio::File::for_path(&path);
            let result = file.trash_future(glib::Priority::DEFAULT).await;
            w.list.reload();
            if let Err(e) = result {
                w.toast(&format!(
                    "Não deu para mover “{}” para a lixeira: {}",
                    file_name(&path),
                    e.message()
                ));
            }
        });
    }
}

/// Favorita a pasta atual, ou tira dos favoritos se já estiver lá.
pub fn toggle_bookmark(win: &Window) {
    let dir = win.current_dir();
    let mut list = win.bookmarks.borrow().clone();
    if !msh_files::bookmarks::remove(&mut list, &dir) {
        msh_files::bookmarks::add(&mut list, dir);
    }
    win.set_bookmarks(list);
}

/// Roda `f` numa thread do GIO, para a janela não travar numa cópia grande.
async fn blocking<T: Send + 'static>(
    f: impl FnOnce() -> io::Result<T> + Send + 'static,
) -> io::Result<T> {
    gio::spawn_blocking(f)
        .await
        .unwrap_or_else(|_| Err(io::Error::other("a operação falhou")))
}

fn file_name(path: &Path) -> String {
    path.file_name().map_or_else(
        || path.to_string_lossy().into_owned(),
        |n| n.to_string_lossy().into_owned(),
    )
}
