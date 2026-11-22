//! `miau-files --self-test`: o "teste com VTE" do PRD §3. Abre a janela de
//! verdade, com o minishell no terminal, e confere a sincronização nos dois
//! sentidos (RF-22) e as ações de arquivo:
//!
//! 1. o shell abre na pasta inicial e anuncia a pasta por OSC 7;
//! 2. navegar na tela digita `cd` no terminal, e o shell vai junto;
//! 3. `cd` digitado no terminal leva a tela junto;
//! 4. com um programa rodando no terminal, a tela não digita nada nele;
//! 5. copiar, colar e renomear (RF-23);
//! 6. favoritar e desfavoritar (RF-23).
//!
//! Em CI, roda sob `xvfb-run`. Sai com 0 se tudo passou e 1 se não.

use std::cell::Cell;
use std::path::{Path, PathBuf};
use std::rc::Rc;
use std::time::{Duration, Instant};

use adw::prelude::*;
use gtk::glib;
use vte::prelude::*;

use crate::actions;
use crate::window::{Origin, ShellSync, Window};

pub fn run(app: &adw::Application, win: Window, result: Rc<Cell<u8>>) {
    let app = app.clone();
    glib::spawn_future_local(async move {
        let outcome = steps(&win).await;
        let code = match outcome {
            Ok(()) => {
                println!("miau-files: autoteste ok");
                0
            }
            Err(msg) => {
                eprintln!("miau-files: autoteste falhou: {msg}");
                1
            }
        };
        result.set(code);
        app.quit();
    });
}

async fn steps(win: &Window) -> Result<(), String> {
    let base = std::env::temp_dir().join(format!("miau-selftest-{}", std::process::id()));
    let sub = base.join("sub dir 'x'");
    std::fs::create_dir_all(&sub).map_err(|e| e.to_string())?;
    std::fs::write(base.join("nota.txt"), "miau").map_err(|e| e.to_string())?;
    let result = check(win, &base, &sub).await;
    let _ = std::fs::remove_dir_all(&base);
    result
}

async fn check(win: &Window, base: &Path, sub: &Path) -> Result<(), String> {
    let start = win.current_dir();
    wait("o shell anunciar a pasta inicial", || {
        win.shell_dir.borrow().as_deref() == Some(start.as_path())
    })
    .await?;
    println!("  1. shell abriu em {}", start.display());

    win.navigate(base, Origin::Screen);
    expect_sync(win, ShellSync::Sent)?;
    wait_shell(win, base).await?;
    wait("a lista mostrar os arquivos", || {
        let names = win.list.names();
        names.iter().any(|n| n == "nota.txt") && names.iter().any(|n| n == "sub dir 'x'")
    })
    .await?;
    win.navigate(sub, Origin::Screen);
    expect_sync(win, ShellSync::Sent)?;
    wait_shell(win, sub).await?;
    println!("  2. tela → shell: {}", sub.display());

    win.terminal.feed_child(b"cd ..\n");
    wait("a tela seguir o cd do shell", || win.current_dir() == base).await?;
    println!("  3. shell → tela: {}", base.display());

    win.terminal.feed_child(b"sleep 2\n");
    let pid = win.shell_pid.get().ok_or("o shell não tem pid")?;
    wait("o sleep começar", || {
        msh_files::proc::has_children(Path::new("/proc"), pid)
    })
    .await?;
    win.navigate(sub, Origin::Screen);
    expect_sync(win, ShellSync::Busy)?;
    wait("o sleep terminar", || {
        !msh_files::proc::has_children(Path::new("/proc"), pid)
    })
    .await?;
    // O prompt seguinte anuncia `base` de novo, e a tela não volta.
    glib::timeout_future(Duration::from_millis(500)).await;
    if win.current_dir() != sub {
        return Err(format!(
            "a tela voltou para {} depois do sleep",
            win.current_dir().display()
        ));
    }
    if win.shell_dir.borrow().as_deref() != Some(base) {
        return Err("o shell mudou de pasta com o terminal ocupado".to_owned());
    }
    println!("  4. terminal ocupado: nada foi digitado nele");

    files(win, base, sub).await?;
    bookmarks(win, base, sub)?;
    if let Some(dir) = std::env::var_os("MIAU_FILES_SELFTEST_ADMIN_DIR") {
        admin_actions(Path::new(&dir)).await?;
    }
    Ok(())
}

/// RF-24, só quando `MIAU_FILES_SELFTEST_ADMIN_DIR` aponta para uma pasta
/// de root e o polkit autoriza este usuário (o CI instala uma regra de
/// teste): o usuário não consegue gravar, e o helper via pkexec consegue.
async fn admin_actions(dir: &Path) -> Result<(), String> {
    use msh_files::helper::Action;
    use std::os::unix::fs::MetadataExt;

    let conf = dir.join("miau.conf");
    match msh_files::ops::write_file(&conf, b"x") {
        Err(e) if e.kind() == std::io::ErrorKind::PermissionDenied => {}
        other => {
            return Err(format!(
                "{} deveria ser só do root: {other:?}",
                dir.display()
            ));
        }
    }
    crate::admin::run(&Action::Write { path: conf.clone() }, Some(b"miau = 1\n")).await?;
    let meta = std::fs::metadata(&conf).map_err(|e| e.to_string())?;
    if std::fs::read_to_string(&conf).ok().as_deref() != Some("miau = 1\n") || meta.uid() != 0 {
        return Err("o helper não gravou o arquivo como root".to_owned());
    }
    let renamed = crate::admin::run(
        &Action::Rename {
            path: conf,
            new_name: "miau2.conf".to_owned(),
        },
        None,
    )
    .await?;
    crate::admin::run(
        &Action::MakeDir {
            path: dir.join("pasta"),
        },
        None,
    )
    .await?;
    if renamed != dir.join("miau2.conf") || !dir.join("pasta").is_dir() {
        return Err("renomear ou criar pasta como root falhou".to_owned());
    }
    println!("  7. ações de administrador via pkexec");
    Ok(())
}

/// RF-23: copiar e colar pela área de transferência, renomear e abrir.
async fn files(win: &Window, base: &Path, sub: &Path) -> Result<(), String> {
    win.navigate(base, Origin::Screen);
    wait("a lista mostrar nota.txt", || {
        win.list.names().iter().any(|n| n == "nota.txt")
    })
    .await?;
    let pos = win
        .list
        .names()
        .iter()
        .position(|n| n == "nota.txt")
        .and_then(|p| u32::try_from(p).ok())
        .ok_or("nota.txt sumiu")?;
    win.list.selection.select_item(pos, true);
    WidgetExt::activate_action(&win.window, "win.copy", None).map_err(|e| e.to_string())?;
    win.navigate(sub, Origin::Screen);
    let pasted = actions::paste(win, sub).await;
    if pasted != [sub.join("nota.txt")] || !sub.join("nota.txt").is_file() {
        return Err(format!("colar deu {pasted:?}"));
    }
    wait("a cópia aparecer na lista", || {
        win.list.names().iter().any(|n| n == "nota.txt")
    })
    .await?;
    let renamed = actions::rename(win, &sub.join("nota.txt"), "renomeada.txt").await;
    if renamed.as_deref() != Some(sub.join("renomeada.txt").as_path()) {
        return Err(format!("renomear deu {renamed:?}"));
    }
    wait("a lista mostrar o novo nome", || {
        let names = win.list.names();
        names.iter().any(|n| n == "renomeada.txt") && !names.iter().any(|n| n == "nota.txt")
    })
    .await?;
    println!("  5. copiar, colar e renomear");
    Ok(())
}

/// RF-23: favoritos na barra lateral, salvos em arquivo.
fn bookmarks(win: &Window, base: &Path, sub: &Path) -> Result<(), String> {
    let file = base.join("config/miau-files/bookmarks");
    win.bookmarks_file.replace(Some(file.clone()));
    win.reload_bookmarks();
    let rows_before = win.sidebar.len();
    actions::toggle_bookmark(win);
    let saved = std::fs::read_to_string(&file).map_err(|e| e.to_string())?;
    if msh_files::bookmarks::parse(&saved) != [msh_files::bookmarks::Bookmark::new(sub.into())] {
        return Err(format!("favoritos salvos: {saved:?}"));
    }
    // Título "Favoritos" + a linha da pasta.
    if win.sidebar.len() != rows_before + 2 {
        return Err("a barra lateral não mostrou o favorito".to_owned());
    }
    actions::toggle_bookmark(win);
    if win.sidebar.len() != rows_before || !win.bookmarks.borrow().is_empty() {
        return Err("o favorito não saiu".to_owned());
    }
    println!("  6. favoritos");
    Ok(())
}

fn expect_sync(win: &Window, want: ShellSync) -> Result<(), String> {
    let got = win.last_sync.get();
    if got == want {
        Ok(())
    } else {
        Err(format!("esperava {want:?} ao navegar, veio {got:?}"))
    }
}

async fn wait_shell(win: &Window, dir: &Path) -> Result<(), String> {
    let want: PathBuf = dir.to_path_buf();
    wait(&format!("o shell ir para {}", dir.display()), || {
        win.shell_dir.borrow().as_ref() == Some(&want)
    })
    .await
}

async fn wait(what: &str, cond: impl Fn() -> bool) -> Result<(), String> {
    let deadline = Instant::now() + Duration::from_secs(15);
    while !cond() {
        if Instant::now() > deadline {
            return Err(format!("tempo esgotado esperando {what}"));
        }
        glib::timeout_future(Duration::from_millis(50)).await;
    }
    Ok(())
}
