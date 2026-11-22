//! `miau-files-helper`: ações de administrador do Miau Files, chamadas pela
//! GUI via `pkexec` (RF-24). Veja `msh_files::helper`.

fn main() {
    let Ok(args) = std::env::args_os()
        .skip(1)
        .map(std::ffi::OsString::into_string)
        .collect::<Result<Vec<_>, _>>()
    else {
        eprintln!("miau-files-helper: argumentos precisam ser UTF-8");
        std::process::exit(2);
    };
    std::process::exit(msh_files::helper::main_with(&args, std::io::stdin().lock()));
}
