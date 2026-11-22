//! `miau-files --self-test`: o "teste com VTE" do PRD §3. Abre a janela de
//! verdade, com o minishell no terminal, e confere a sincronização nos dois
//! sentidos (RF-22):
//!
//! 1. o shell abre na pasta inicial e anuncia a pasta por OSC 7;
//! 2. navegar na tela digita `cd` no terminal, e o shell vai junto;
//! 3. `cd` digitado no terminal leva a tela junto;
//! 4. com um programa rodando no terminal, a tela não digita nada nele.
//!
//! Em CI, roda sob `xvfb-run`. Sai com 0 se tudo passou e 1 se não.

use std::cell::Cell;
use std::path::{Path, PathBuf};
use std::rc::Rc;
use std::time::{Duration, Instant};

use adw::prelude::*;
use gtk::glib;
use vte::prelude::*;

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
