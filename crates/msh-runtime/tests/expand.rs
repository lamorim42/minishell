//! Expansão de palavras (POSIX 2.6). Os resultados esperados batem com o
//! `bash --posix` e o dash.

use msh_runtime::{Env, Shell, expand_redirect_target, expand_words};
use msh_syntax::{Command, Node, Redirect, parse};
use proptest::prelude::*;

fn shell(vars: &[(&str, &str)]) -> Shell {
    Shell::new(Env::from_pairs(vars.iter().copied()))
}

/// Expande o `argv` da linha, como o shell faria antes de executar.
fn argv(src: &str, sh: &Shell) -> Vec<String> {
    match parse(src).unwrap().unwrap() {
        Node::Pipeline(p) => match &p.commands[0] {
            Command::Simple(cmd) => expand_words(&cmd.argv, sh),
            Command::Subshell { .. } => unreachable!(),
        },
        _ => unreachable!(),
    }
}

fn redirect_target(src: &str, sh: &Shell) -> String {
    let Some(Node::Pipeline(p)) = parse(src).unwrap() else {
        unreachable!()
    };
    let Command::Simple(cmd) = &p.commands[0] else {
        unreachable!()
    };
    match &cmd.redirects[0] {
        Redirect::In(w) | Redirect::Out(w) | Redirect::Append(w) => expand_redirect_target(w, sh),
        Redirect::HereDoc { .. } => unreachable!(),
    }
}

#[test]
fn variables_and_quotes() {
    let sh = shell(&[("A", "a  b"), ("USER", "luis")]);
    assert_eq!(argv("echo $A", &sh), ["echo", "a", "b"]);
    assert_eq!(argv("echo \"$A\"", &sh), ["echo", "a  b"]);
    assert_eq!(argv("echo '$A'", &sh), ["echo", "$A"]);
    assert_eq!(argv("echo \"oi $USER\"!", &sh), ["echo", "oi luis!"]);
    assert_eq!(argv("echo ${USER}x $USERx", &sh), ["echo", "luisx"]);
    assert_eq!(argv(r"echo \$USER", &sh), ["echo", "$USER"]);
    assert_eq!(argv("echo $ a$", &sh), ["echo", "$", "a$"]);
}

#[test]
fn empty_values() {
    let sh = shell(&[("EMPTY", "")]);
    // Sem aspas, vazio some; com aspas, vira argumento vazio.
    assert_eq!(argv("echo $EMPTY", &sh), ["echo"]);
    assert_eq!(argv("echo $NAO_EXISTE", &sh), ["echo"]);
    assert_eq!(argv("echo \"$EMPTY\"", &sh), ["echo", ""]);
    assert_eq!(argv("echo ''", &sh), ["echo", ""]);
    assert_eq!(argv("echo a$EMPTY", &sh), ["echo", "a"]);
    assert_eq!(argv("$EMPTY", &sh), Vec::<String>::new());
}

#[test]
fn ported_c_cases() {
    // Casos de tests_tokenizer.cpp, agora com a expansão.
    let sh = shell(&[("HOME", "/home/luis"), ("USER", "luis")]);
    assert_eq!(
        argv("echo \"\"$HOME\"\" ''$USER''", &sh),
        ["echo", "/home/luis", "luis"]
    );
    assert_eq!(
        argv("echo 'oi '$USER', \"bem vindo\"'", &sh),
        ["echo", "oi luis, \"bem vindo\""]
    );
}

#[test]
fn last_status_and_special_params() {
    let mut sh = shell(&[]);
    sh.last_status = 127;
    assert_eq!(
        argv("echo $? \"$?\"x ${?}", &sh),
        ["echo", "127", "127x", "127"]
    );
    assert_eq!(argv("echo $0 $#", &sh), ["echo", "minishell", "0"]);
    assert_eq!(argv("echo $1 $@ $*", &sh), ["echo"]);
    assert_eq!(
        argv("echo $$", &sh),
        ["echo", &std::process::id().to_string()]
    );
}

#[test]
fn field_splitting_with_custom_ifs() {
    let sh = shell(&[("IFS", ","), ("A", "x,,y,")]);
    assert_eq!(argv("echo $A", &sh), ["echo", "x", "", "y"]);

    let sh = shell(&[("IFS", " ,"), ("A", " a , b ,c ")]);
    assert_eq!(argv("echo $A", &sh), ["echo", "a", "b", "c"]);

    let sh = shell(&[("IFS", ","), ("A", ",b")]);
    assert_eq!(argv("echo $A", &sh), ["echo", "", "b"]);

    // IFS vazio: nada é dividido.
    let sh = shell(&[("IFS", ""), ("A", "a b")]);
    assert_eq!(argv("echo $A", &sh), ["echo", "a b"]);
}

#[test]
fn literal_text_is_never_split() {
    let sh = shell(&[("IFS", "x"), ("A", "1x2")]);
    assert_eq!(argv("echo axb $A", &sh), ["echo", "axb", "1", "2"]);
    let sh = shell(&[("A", "x ")]);
    assert_eq!(argv("echo $A\"b\"", &sh), ["echo", "x", "b"]);
}

#[test]
fn tilde() {
    let sh = shell(&[("HOME", "/home/ana")]);
    assert_eq!(
        argv("echo ~ ~/x a~ '~' \"~\" ~root", &sh),
        ["echo", "/home/ana", "/home/ana/x", "a~", "~", "~", "/root"]
    );
    assert_eq!(
        argv("echo ~usuario_que_nao_existe", &sh),
        ["echo", "~usuario_que_nao_existe"]
    );
    // O resultado do til não passa por field splitting.
    let sh = shell(&[("HOME", "/a b")]);
    assert_eq!(argv("echo ~", &sh), ["echo", "/a b"]);
    // Sem HOME, o til fica literal.
    assert_eq!(argv("echo ~", &shell(&[])), ["echo", "~"]);
}

#[test]
fn redirect_target_is_not_field_split() {
    // POSIX 2.7: o alvo não passa por field splitting (como no dash).
    let sh = shell(&[("A", "a b"), ("F", "out.txt"), ("EMPTY", "")]);
    assert_eq!(redirect_target("> $F", &sh), "out.txt");
    assert_eq!(redirect_target("> \"$A\"", &sh), "a b");
    assert_eq!(redirect_target("> $A", &sh), "a b");
    assert_eq!(redirect_target("< $EMPTY", &sh), "");
    assert_eq!(redirect_target(">> x$F", &sh), "xout.txt");
}

proptest! {
    /// Com o IFS padrão, `$V` sem aspas se divide como `split_whitespace`, e
    /// `"$V"` fica intacto.
    #[test]
    fn default_ifs_matches_split_whitespace(value in "[ab \t\n]{0,16}") {
        let sh = shell(&[("V", &value)]);
        let expected: Vec<_> = value.split_ascii_whitespace().collect();
        prop_assert_eq!(argv("$V", &sh), expected);
        prop_assert_eq!(argv("\"$V\"", &sh), vec![value.clone()]);
    }
}
