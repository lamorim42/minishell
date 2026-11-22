//! M4: pipes (POSIX 2.9.2), redirecionamentos e heredoc (2.7), e nenhum fd
//! vazando para os filhos.

mod common;

use std::path::Path;

use common::case;

fn file(dir: &Path, name: &str, body: &str) {
    std::fs::write(dir.join(name), body).unwrap();
}

// --- pipes ---------------------------------------------------------------------

#[test]
fn pipes() {
    case("echo oi | cat").check();
    case("echo oi | tr a-z A-Z | rev").check();
    case("printf 'b\\na\\nc\\n' | sort | head -n 2").check();
    case("yes | head -n 3").check();
    case("ls / | wc -l").check();
    case("cat nao_existe | wc -l").check();
    case("echo x | nao_existe").check();
}

#[test]
fn pipeline_status_is_the_last_command() {
    case("false | true\necho $?").check();
    case("true | false\necho $?").check();
    case("true | sh -c 'exit 9'\necho $?").check();
}

#[test]
fn builtins_in_pipeline_run_in_a_subshell() {
    // POSIX 2.9.2: cada comando de uma pipeline com mais de um comando roda
    // num subshell; nada muda no shell.
    case("cd / | true\npwd").check();
    case("export A=1 | true\necho [$A]").check();
    case("exit 3 | echo ainda aqui\necho $?").check();
    case("echo oi | cat | echo fim").check();
    case("pwd | cat").check();
    case("export | grep -c PATH").check();
}

#[test]
fn stdin_of_shell_is_shared_by_first_stage() {
    case("cat | tr a-z A-Z").stdin("de fora\n").check();
}

// --- redirecionamentos ---------------------------------------------------------

#[test]
fn output_redirections() {
    case("echo a > f\necho b >> f\ncat f").check();
    case("echo a > f\necho b > f\ncat f").check();
    case("> vazio\nls\ncat vazio").check();
    case("echo x > a > b\ncat a\necho ---\ncat b").check();
    case("echo $HOME > \"$HOME/caminho com espaco\"\ncat \"caminho com espaco\"").check();
}

#[test]
fn input_redirections() {
    let setup = |d: &Path| file(d, "in", "linha 1\nlinha 2\n");
    case("cat < in").setup(&setup).check();
    case("wc -l < in > out\ncat out").setup(&setup).check();
    case("< in cat").setup(&setup).check();
    case("cat < in | wc -l").setup(&setup).check();
    // Erro de redirecionamento: o POSIX só pede status >0 (2.8.1). O dash
    // dá 2; o minishell dá 1, como o bash.
    case("cat < nao_existe\necho $?")
        .expect("1\n", 0)
        .any_stderr()
        .check();
}

#[test]
fn redirection_errors() {
    case("echo x > /nao/existe\necho $?")
        .expect("1\n", 0)
        .any_stderr()
        .check();
    case("cat < /\necho $?").any_stderr().check();
    // POSIX 2.7: o alvo não sofre field splitting. O dash cria "a b"; o
    // bash recusa com "ambiguous redirect".
    case("A='a b'\necho x > $A\necho $?\ncat 'a b'")
        .expect("0\nx\n", 0)
        .check();
    case("echo x > $NADA\necho $?")
        .expect("1\n", 0)
        .any_stderr()
        .check();
    case("echo x > ''\necho $?")
        .expect("1\n", 0)
        .any_stderr()
        .check();
}

#[test]
fn builtin_redirection_is_undone() {
    // RE-01 da unit: o builtin roda no pai, e o redirecionamento é desfeito.
    case("cd / > /dev/null\npwd").check();
    case("pwd > f\necho depois\ncat f").check();
    case("echo -n oi > f\necho !\ncat f").check();
    case("export > f\ngrep -c PATH f").check();
    case("cd / < nao_existe\npwd").check();
}

#[test]
fn redirect_only_with_assignments() {
    case("A=1 > f\necho $A\nls").check();
}

// --- heredoc -------------------------------------------------------------------

#[test]
fn heredoc_expands_without_quotes() {
    case("A=mundo\ncat <<EOF\nola $A ${A}!\n$? \\$A\nEOF").check();
    case("cat <<EOF\na\\\\b \\\"c\\\" 'd'\nEOF").check();
    case("cat <<EOF\ncusta $ 5\nEOF").check();
}

#[test]
fn heredoc_quoted_delimiter_is_literal() {
    case("cat <<'EOF'\n$HOME \\$x\nEOF").check();
    case("cat <<\"E\"\n$HOME\nE").check();
    case("cat <<E\\F\n$HOME\nEF").check();
}

#[test]
fn heredoc_in_pipelines_and_sequences() {
    case("cat <<A | tr a-z A-Z\nabc\nA").check();
    case("cat <<A <<B\num\nA\ndois\nB").check();
    case("cat <<A | cat <<B\num\nA\ndois\nB").check();
    case("cat <<E > f\nconteudo\nE\ncat f").check();
    case("cat <<E\nE\necho vazio").check();
    case("cat <<E\n  E\nE").check();
}

#[test]
fn heredoc_at_end_of_input() {
    // O POSIX não define o fim de entrada no meio de um heredoc. O minishell
    // faz como o bash: avisa e usa as linhas lidas, cada uma com `\n` (o dash
    // perde o `\n` da última linha do `-c`).
    case("cat <<E\nsem fim")
        .expect("sem fim\n", 0)
        .any_stderr()
        .check();
}

#[test]
fn large_heredoc_does_not_block() {
    // Mais que os 64 KiB de um pipe: o memfd não tem limite (ADR-009).
    let body = "x".repeat(100) + "\n";
    let script = format!("cat <<E | wc -c\n{}E", body.repeat(1000));
    case(&script).check();
}

// --- fds (RNF-10) --------------------------------------------------------------

#[test]
fn no_fd_leaks_to_children() {
    // Só 0, 1 e 2 chegam ao filho; o 3 é o diretório que o próprio `ls` abre.
    case("ls /proc/self/fd").expect("0\n1\n2\n3\n", 0).check();
    case("ls /proc/self/fd | cat")
        .expect("0\n1\n2\n3\n", 0)
        .check();
    case("echo x | ls /proc/self/fd")
        .expect("0\n1\n2\n3\n", 0)
        .check();
    case("cat <<E | ls /proc/self/fd\nx\nE")
        .expect("0\n1\n2\n3\n", 0)
        .check();
    case("ls /proc/self/fd < /dev/null > f\ncat f")
        .expect("0\n1\n2\n3\n", 0)
        .check();
    case("pwd > /dev/null\nls /proc/self/fd")
        .expect("0\n1\n2\n3\n", 0)
        .check();
}
