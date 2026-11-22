//! M3: comando simples, `PATH`, status 126/127/2, atribuições e os builtins
//! `echo`, `cd`, `pwd` e `exit`. Cada caso roda no minishell, no dash e no
//! `bash --posix` (veja `common/mod.rs`).

mod common;

use std::os::unix::fs::PermissionsExt;
use std::path::Path;

use common::case;

fn write_file(dir: &Path, name: &str, body: &str, mode: u32) {
    let path = dir.join(name);
    std::fs::write(&path, body).unwrap();
    std::fs::set_permissions(&path, std::fs::Permissions::from_mode(mode)).unwrap();
}

// --- execução e PATH ---------------------------------------------------------

#[test]
fn runs_programs_from_path_and_by_path() {
    case("echo oi").check();
    case("ls /").check();
    case("/bin/echo por caminho absoluto").check();
    case("printf '%s-' a b c").check();
    case("true").check();
    case("false").check();
}

#[test]
fn command_not_found_is_127() {
    case("comando_que_nao_existe").check();
    case("comando_que_nao_existe\necho $?").check();
    case("/nao/existe").check();
    case("''").check();
}

#[test]
fn not_executable_is_126() {
    // Diretório: o bash diz "Is a directory"; o dash, "Permission denied".
    case("/").check();
    let setup = |d: &Path| write_file(d, "f", "echo x\n", 0o644);
    case("./f").setup(&setup).check();
}

#[test]
fn non_executable_in_path_is_not_found() {
    // XBD 8.3: a busca no PATH procura "an executable file". Um arquivo sem
    // permissão não conta, então o resultado é 127, como no dash. O bash
    // dá 126 ("Permission denied").
    let setup = |d: &Path| write_file(d, "naoexec", "echo x\n", 0o644);
    case("PATH=.\nnaoexec")
        .setup(&setup)
        .expect("", 127)
        .check();
}

#[test]
fn file_without_shebang_runs_as_script() {
    // POSIX 2.9.1.6: ENOEXEC faz o shell rodar o arquivo como script.
    let setup = |d: &Path| write_file(d, "s", "echo de dentro do script\necho $?\n", 0o755);
    case("./s").setup(&setup).check();
}

#[test]
fn empty_expansion_is_no_command() {
    case("$NADA\necho $?").check();
    case("false\n$NADA\necho $?").check();
}

// --- status e erros de sintaxe ----------------------------------------------

#[test]
fn last_status() {
    case("false\necho $?").check();
    case("true\necho $?").check();
    case("sh -c 'exit 7'\necho $?").check();
}

#[test]
fn syntax_error_is_2_and_ends_non_interactive_shell() {
    case("echo |").check();
    // O bash mostra a linha com erro numa segunda linha de stderr.
    case("| echo").any_stderr().check();
    case("echo antes\necho >\necho depois").any_stderr().check();
}

// --- atribuições (POSIX 2.9.1) -----------------------------------------------

#[test]
fn assignments() {
    case("A=1\necho $A").check();
    case("A='x  y'\necho $A \"$A\"").check();
    case("A=1 B=2\necho $A$B").check();
    case("A=1 sh -c 'echo $A'\necho [$A]").check();
    case("A=$HOME\necho $A").check();
    case("export A=1\nsh -c 'echo $A'").check();
    case("A=1\nexport A\nsh -c 'echo $A'").check();
    // Antes de builtin comum, vale só durante ele.
    case("A=1 pwd\necho [$A]").check();
    // Antes de builtin especial, fica no shell.
    case("A=1 export B=2\necho $A $B").check();
    case("\"A\"=1").check();
}

// --- echo ----------------------------------------------------------------------

#[test]
fn echo() {
    case("echo").check();
    case("echo a b   c").check();
    case("echo a \"\" b").check();
    case("echo -n sem quebra").check();
    case("echo -n").check();
    case("echo -- -n").check();
}

#[test]
fn echo_follows_dash_where_posix_is_open() {
    // POSIX: `-n` e `\` são definidos pela implementação; ADR-013 manda
    // seguir o dash. O bash --posix trata `-nn` como opção e não interpreta
    // `\t`.
    case("echo -nn z").expect("-nn z\n", 0).check();
    case("echo -n -n z").expect("-n z", 0).check();
    case("echo 'a\\tb'").expect("a\tb\n", 0).check();
    case("echo 'corta\\c' aqui").expect("corta", 0).check();
}

// --- cd e pwd ----------------------------------------------------------------

#[test]
fn cd_and_pwd() {
    case("pwd").check();
    case("cd /\npwd").check();
    case("cd /usr/bin\ncd ..\npwd").check();
    case("cd /\ncd\npwd").check();
    case("cd /\necho $OLDPWD $PWD").check();
    case("cd /usr\ncd /\ncd -\npwd").check();
    // Falha do cd: o POSIX só pede ">0"; o dash dá 2, o bash 1. Seguimos o
    // bash (status de erro comum dos utilitários).
    case("cd nao_existe\necho $?")
        .expect("1\n", 0)
        .any_stderr()
        .check();
    case("cd -P /\npwd -P").check();
}

#[test]
fn cd_logical_and_physical() {
    let setup = |d: &Path| {
        std::fs::create_dir(d.join("real")).unwrap();
        std::os::unix::fs::symlink("real", d.join("link")).unwrap();
    };
    case("cd link\npwd\npwd -P").setup(&setup).check();
    // `pwd -P` não muda o PWD (o bash muda; o dash e o POSIX, não).
    case("cd link\npwd -P\necho $PWD")
        .setup(&setup)
        .expect("$TMP/real\n$TMP/link\n", 0)
        .check();
    case("cd -P link\npwd").setup(&setup).check();
    case("cd link\ncd ..\npwd").setup(&setup).check();
}

#[test]
fn cd_decisions() {
    // HOME ausente: comportamento definido pela implementação (POSIX `cd`).
    // Seguimos o bash: erro e status 1 (o dash não faz nada e dá 0).
    case("unset HOME\ncd\necho $?")
        .expect("1\n", 0)
        .any_stderr()
        .check();
    // Mais de um operando: não especificado. O dash ignora o resto; o
    // minishell recusa, como o bash.
    case("cd / /usr\necho $?")
        .expect("1\n", 0)
        .any_stderr()
        .check();
    case("unset OLDPWD\ncd -\necho $?")
        .expect("1\n", 0)
        .any_stderr()
        .check();
}

// --- exit --------------------------------------------------------------------

#[test]
fn exit() {
    case("exit").check();
    case("exit 3").check();
    case("exit 300").check();
    case("exit +7").check();
    case("false\nexit").check();
    case("exit 5\necho nunca").check();
    case("exit 1 2\necho nunca").check();
}

#[test]
fn exit_numeric_argument_required() {
    // O dash escreve "Illegal number"; a mensagem segue o bash.
    case("exit abc\necho nunca").check();
    // Negativo: o POSIX não define; seguimos o dash (o bash sai com 255).
    case("exit -1\necho nunca")
        .expect("", 2)
        .any_stderr()
        .check();
}

#[test]
fn special_builtin_error_ends_non_interactive_shell() {
    // POSIX 2.8.1: erro num special built-in encerra o shell não interativo
    // (o dash sai; o bash --posix continua).
    case("export 1a\necho nunca")
        .expect("", 1)
        .any_stderr()
        .check();
    case("unset 1a\necho nunca")
        .expect("", 1)
        .any_stderr()
        .check();
}
