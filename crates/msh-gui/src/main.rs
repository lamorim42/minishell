//! Miau Files (`miau-files`): navegador de arquivos em GTK4 + libadwaita com
//! o minishell embutido num terminal VTE (fase 2, ADR-011, TDD §7).
//!
//! ```text
//! miau-files [PASTA]       abre a janela em PASTA (padrão: a pasta atual)
//! miau-files --self-test   abre a janela, testa a sincronização tela ↔
//!                          shell nos dois sentidos e sai com 0 ou 1
//! ```
//!
//! A GUI e o shell só conversam pelo PTY: a tela digita `cd '...'` no
//! terminal, e o shell avisa o diretório com OSC 7 a cada prompt.

mod files;
mod selftest;
mod sidebar;
mod terminal;
mod window;

use std::path::PathBuf;
use std::rc::Rc;

use adw::prelude::*;

const APP_ID: &str = "org.miaushell.Files";
const USAGE: &str = "usage: miau-files [--self-test] [DIR]";

#[derive(Debug, Default)]
struct Options {
    dir: Option<PathBuf>,
    self_test: bool,
}

fn main() -> std::process::ExitCode {
    // PRD §3: nenhum processo da GUI roda como root. O que precisa de root
    // passa pelo helper do polkit (RF-24).
    if msh_files::proc::running_as_root() {
        eprintln!(
            "miau-files: não rode a interface como root; as ações de administrador pedem a senha quando precisam"
        );
        return std::process::ExitCode::from(1);
    }
    let opts = match parse_args(std::env::args().skip(1)) {
        Ok(o) => o,
        Err(msg) => {
            eprintln!("miau-files: {msg}\n{USAGE}");
            return std::process::ExitCode::from(2);
        }
    };
    let start = opts
        .dir
        .clone()
        .or_else(|| std::env::current_dir().ok())
        .unwrap_or_else(|| PathBuf::from("/"));
    let start = std::fs::canonicalize(&start).unwrap_or(start);

    let app = adw::Application::builder()
        .application_id(APP_ID)
        .flags(gio::ApplicationFlags::NON_UNIQUE)
        .build();
    let result = Rc::new(std::cell::Cell::new(0_u8));
    let self_test = opts.self_test;
    let res = result.clone();
    app.connect_activate(move |app| {
        let win = window::Window::new(app, &start);
        win.present();
        if self_test {
            selftest::run(app, win, res.clone());
        }
    });
    // O GApplication não vê nossos argumentos: eles já foram lidos acima.
    let code = app.run_with_args(&["miau-files"]);
    if code != gtk::glib::ExitCode::SUCCESS {
        return std::process::ExitCode::from(1);
    }
    std::process::ExitCode::from(result.get())
}

fn parse_args(args: impl Iterator<Item = String>) -> Result<Options, String> {
    let mut opts = Options::default();
    for arg in args {
        match arg.as_str() {
            "--self-test" => opts.self_test = true,
            "-h" | "--help" => return Err("ajuda".to_owned()),
            s if s.starts_with('-') => return Err(format!("{s}: opção inválida")),
            _ if opts.dir.is_some() => return Err("só uma pasta por vez".to_owned()),
            _ => opts.dir = Some(PathBuf::from(arg)),
        }
    }
    Ok(opts)
}

use gtk::gio;

#[cfg(test)]
mod tests {
    use super::*;

    fn parse(list: &[&str]) -> Result<Options, String> {
        parse_args(list.iter().map(|&s| s.to_owned()))
    }

    #[test]
    fn args() {
        let o = parse(&["/etc"]).unwrap();
        assert_eq!(o.dir, Some(PathBuf::from("/etc")));
        assert!(!o.self_test);
        assert!(parse(&["--self-test"]).unwrap().self_test);
        assert!(parse(&["-x"]).is_err());
        assert!(parse(&["/a", "/b"]).is_err());
    }
}
