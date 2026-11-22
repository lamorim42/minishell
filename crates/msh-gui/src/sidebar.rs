//! Barra lateral (RF-20, RF-23): locais fixos (Pasta pessoal, `/`, `/etc`,
//! `/var/log`, TDD §7) e os favoritos do usuário.

use std::cell::RefCell;
use std::path::{Path, PathBuf};
use std::rc::Rc;

use gtk::glib;
use gtk::prelude::*;

#[derive(Debug, Clone)]
pub struct Sidebar {
    pub list: gtk::ListBox,
    pub scrolled: gtk::ScrolledWindow,
    /// O caminho de cada linha, na mesma ordem da lista.
    rows: Rc<RefCell<Vec<Option<PathBuf>>>>,
}

impl Sidebar {
    pub fn new() -> Self {
        let list = gtk::ListBox::builder()
            .selection_mode(gtk::SelectionMode::Single)
            .build();
        list.add_css_class("navigation-sidebar");
        let scrolled = gtk::ScrolledWindow::builder()
            .child(&list)
            .hscrollbar_policy(gtk::PolicyType::Never)
            .width_request(180)
            .build();
        let bar = Self {
            list,
            scrolled,
            rows: Rc::default(),
        };
        bar.fill_places();
        bar
    }

    fn fill_places(&self) {
        let places = [
            ("user-home-symbolic", "Pasta pessoal", glib::home_dir()),
            ("drive-harddisk-symbolic", "Computador", PathBuf::from("/")),
            ("emblem-system-symbolic", "/etc", PathBuf::from("/etc")),
            (
                "text-x-generic-symbolic",
                "/var/log",
                PathBuf::from("/var/log"),
            ),
        ];
        self.header("Locais");
        for (icon, title, path) in places {
            self.push_row(icon, title, path);
        }
    }

    /// Um título de seção (linha que não se pode ativar).
    pub fn header(&self, title: &str) {
        let label = gtk::Label::builder()
            .label(title)
            .xalign(0.0)
            .margin_top(8)
            .build();
        label.add_css_class("heading");
        label.add_css_class("dim-label");
        let row = gtk::ListBoxRow::builder()
            .child(&label)
            .activatable(false)
            .selectable(false)
            .build();
        self.list.append(&row);
        self.rows.borrow_mut().push(None);
    }

    pub fn push_row(&self, icon: &str, title: &str, path: PathBuf) -> gtk::Box {
        let b = gtk::Box::new(gtk::Orientation::Horizontal, 10);
        b.append(&gtk::Image::from_icon_name(icon));
        b.append(
            &gtk::Label::builder()
                .label(title)
                .xalign(0.0)
                .hexpand(true)
                .ellipsize(gtk::pango::EllipsizeMode::End)
                .build(),
        );
        let row = gtk::ListBoxRow::builder().child(&b).build();
        row.set_tooltip_text(Some(&path.to_string_lossy()));
        self.list.append(&row);
        self.rows.borrow_mut().push(Some(path));
        b
    }

    /// Uma linha de favorito, com um botão para remover. Devolve o botão.
    pub fn push_bookmark(&self, title: &str, path: PathBuf) -> gtk::Button {
        let b = self.push_row("starred-symbolic", title, path);
        let remove = gtk::Button::builder()
            .icon_name("list-remove-symbolic")
            .tooltip_text("Remover dos favoritos")
            .build();
        remove.add_css_class("flat");
        b.append(&remove);
        remove
    }

    /// Apaga as linhas a partir da posição `from` (usado para refazer a
    /// seção de favoritos).
    pub fn truncate(&self, from: usize) {
        while self.rows.borrow().len() > from {
            let last = i32::try_from(self.rows.borrow().len() - 1).unwrap_or(i32::MAX);
            if let Some(row) = self.list.row_at_index(last) {
                self.list.remove(&row);
            }
            self.rows.borrow_mut().pop();
        }
    }

    pub fn len(&self) -> usize {
        self.rows.borrow().len()
    }

    pub fn path_of(&self, row: &gtk::ListBoxRow) -> Option<PathBuf> {
        let i = usize::try_from(row.index()).ok()?;
        self.rows.borrow().get(i).cloned().flatten()
    }

    /// Marca a linha de `path`, se houver uma.
    pub fn select(&self, path: &Path) {
        let index = self
            .rows
            .borrow()
            .iter()
            .position(|p| p.as_deref() == Some(path));
        match index.and_then(|i| self.list.row_at_index(i32::try_from(i).ok()?)) {
            Some(row) => self.list.select_row(Some(&row)),
            None => self.list.unselect_all(),
        }
    }
}
