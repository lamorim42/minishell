//! Ações de administrador (RF-24, TDD §7): a GUI continua como usuário
//! comum e chama `pkexec miau-files-helper <ação>`. O polkit pede a senha
//! (ação `org.miaushell.files.helper`, com `auth_admin_keep`: uma senha vale
//! por alguns minutos) e só o helper roda como root.

use std::path::{Path, PathBuf};

use adw::prelude::*;
use gtk::{gio, glib};
use msh_files::helper::Action;

use crate::window::Window;

/// Onde o helper foi instalado pelo `.deb`. A política do polkit aponta
/// para este caminho.
pub const INSTALLED_HELPER: &str = "/usr/libexec/miau-files/miau-files-helper";

/// `$MIAU_FILES_HELPER`, o helper instalado ou, em desenvolvimento, o que
/// está ao lado do `miau-files` em `target/`.
pub fn helper_program() -> PathBuf {
    if let Some(p) = std::env::var_os("MIAU_FILES_HELPER") {
        return PathBuf::from(p);
    }
    if Path::new(INSTALLED_HELPER).is_file() {
        return PathBuf::from(INSTALLED_HELPER);
    }
    std::env::current_exe()
        .ok()
        .and_then(|exe| exe.parent().map(|d| d.join("miau-files-helper")))
        .filter(|p| p.is_file())
        .unwrap_or_else(|| PathBuf::from(INSTALLED_HELPER))
}

/// A linha de comando do `pkexec` para `action`.
pub fn pkexec_argv(helper: &Path, action: &Action) -> Vec<String> {
    let mut argv = vec!["pkexec".to_owned(), helper.to_string_lossy().into_owned()];
    argv.extend(action.to_args());
    argv
}

/// Roda `action` como root. `stdin` vai para o helper (o conteúdo de
/// `write`). Devolve o caminho criado ou uma mensagem de erro.
pub async fn run(action: &Action, stdin: Option<&[u8]>) -> Result<PathBuf, String> {
    let argv = pkexec_argv(&helper_program(), action);
    let argv: Vec<&std::ffi::OsStr> = argv.iter().map(std::ffi::OsStr::new).collect();
    let flags = gio::SubprocessFlags::STDIN_PIPE
        | gio::SubprocessFlags::STDOUT_PIPE
        | gio::SubprocessFlags::STDERR_PIPE;
    let proc = gio::Subprocess::newv(&argv, flags).map_err(|e| {
        format!(
            "não deu para chamar o pkexec ({}); instale o pacote pkexec",
            e.message()
        )
    })?;
    let input = glib::Bytes::from(stdin.unwrap_or_default());
    let (out, err) = proc
        .communicate_future(Some(&input))
        .await
        .map_err(|e| e.message().to_owned())?;
    let text = |b: Option<glib::Bytes>| {
        b.map(|b| String::from_utf8_lossy(&b).trim().to_owned())
            .unwrap_or_default()
    };
    match proc.exit_status() {
        0 => Ok(PathBuf::from(text(out))),
        // pkexec: 126 = o usuário cancelou, 127 = não autorizado.
        126 | 127 => Err("autorização negada ou cancelada".to_owned()),
        _ => {
            let msg = text(err);
            Err(msg
                .strip_prefix("miau-files-helper: ")
                .unwrap_or(&msg)
                .to_owned())
        }
    }
}

/// Pergunta se a ação que falhou por permissão deve ser refeita como
/// administrador. Devolve `true` se o usuário aceitar.
pub async fn ask(win: &Window, what: &str) -> bool {
    let dialog = adw::AlertDialog::new(
        Some("Precisa de permissão de administrador"),
        Some(&format!(
            "Você não tem permissão para {what}. Dá para tentar como administrador: a senha é pedida e só esta ação roda como root."
        )),
    );
    dialog.add_response("cancel", "Cancelar");
    dialog.add_response("admin", "Tentar como administrador");
    dialog.set_response_appearance("admin", adw::ResponseAppearance::Suggested);
    dialog.set_default_response(Some("admin"));
    dialog.set_close_response("cancel");
    dialog.choose_future(Some(&win.window)).await == "admin"
}

/// Roda `action` como administrador depois de perguntar, e avisa o
/// resultado num toast.
pub async fn retry(win: &Window, what: &str, action: Action) {
    if !ask(win, what).await {
        return;
    }
    match run(&action, None).await {
        Ok(path) => {
            win.list.reload();
            win.toast(&format!("Feito como administrador: {}", path.display()));
        }
        Err(e) => win.toast(&format!("Não deu certo: {e}")),
    }
}

/// "Editar como administrador": um editor de texto simples que salva pelo
/// helper (`write`). Serve para arquivos como os de `/etc`, que se pode ler
/// mas não gravar.
pub fn edit(win: &Window, path: &Path) {
    let content = match std::fs::metadata(path) {
        Ok(m) if m.len() > msh_files::ops::MAX_WRITE => {
            return win.toast("Arquivo grande demais para o editor (limite de 16 MiB)");
        }
        Ok(m) if !m.is_file() => return win.toast("Só dá para editar arquivos comuns"),
        Ok(_) => std::fs::read(path),
        Err(e) => Err(e),
    };
    let text = match content {
        Ok(bytes) => match String::from_utf8(bytes) {
            Ok(t) => t,
            Err(_) => return win.toast("O arquivo não é texto (UTF-8)"),
        },
        Err(e) => {
            return win.toast(&format!(
                "Não deu para ler o arquivo: {}",
                msh_files::format::io_error(&e)
            ));
        }
    };

    let buffer = gtk::TextBuffer::new(None);
    buffer.set_text(&text);
    let view = gtk::TextView::builder()
        .buffer(&buffer)
        .monospace(true)
        .top_margin(8)
        .bottom_margin(8)
        .left_margin(8)
        .right_margin(8)
        .build();
    let save = gtk::Button::builder()
        .label("Salvar como administrador")
        .build();
    save.add_css_class("suggested-action");
    let header = adw::HeaderBar::new();
    header.pack_end(&save);
    let toasts = adw::ToastOverlay::new();
    toasts.set_child(Some(
        &gtk::ScrolledWindow::builder()
            .child(&view)
            .vexpand(true)
            .build(),
    ));
    let content = adw::ToolbarView::new();
    content.add_top_bar(&header);
    content.set_content(Some(&toasts));
    let editor = adw::Window::builder()
        .title(format!("{} (administrador)", path.display()))
        .transient_for(&win.window)
        .default_width(720)
        .default_height(520)
        .content(&content)
        .build();

    let path = path.to_path_buf();
    save.connect_clicked(move |button| {
        let text = buffer.text(&buffer.start_iter(), &buffer.end_iter(), true);
        let action = Action::Write { path: path.clone() };
        let toasts = toasts.clone();
        let button = button.clone();
        button.set_sensitive(false);
        glib::spawn_future_local(async move {
            let msg = match run(&action, Some(text.as_bytes())).await {
                Ok(_) => "Salvo".to_owned(),
                Err(e) => format!("Não salvou: {e}"),
            };
            toasts.add_toast(adw::Toast::new(&msg));
            button.set_sensitive(true);
        });
    });
    editor.present();
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn pkexec_line() {
        let argv = pkexec_argv(
            Path::new(INSTALLED_HELPER),
            &Action::Rename {
                path: "/etc/a b".into(),
                new_name: "c".into(),
            },
        );
        assert_eq!(
            argv,
            ["pkexec", INSTALLED_HELPER, "rename", "/etc/a b", "c"]
        );
    }
}
