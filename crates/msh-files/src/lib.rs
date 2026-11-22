//! Lógica do Miau Files (fase 2) que não depende do GTK.
//!
//! O crate fica separado do `msh-gui` por dois motivos:
//!
//! - tudo aqui é testável sem tela (formatação, URIs do OSC 7, favoritos,
//!   histórico de navegação e operações de arquivo);
//! - o helper de administrador (`miau-files-helper`, RF-24) roda como root
//!   via polkit e precisa ser pequeno: ele usa só este crate, sem linkar o
//!   GTK (ADR-011, TDD §7).

pub mod bookmarks;
pub mod format;
pub mod helper;
pub mod history;
pub mod ops;
pub mod paths;
pub mod proc;
pub mod uri;
