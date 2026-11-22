//! Lista de arquivos (RF-20): `gtk::ColumnView` sobre um
//! `gtk::DirectoryList` (GIO). O GIO acompanha a pasta com inotify, então a
//! lista se atualiza sozinha quando algo muda, inclusive pelo terminal.
//!
//! Colunas: nome, tamanho, dono, permissões (`rwxr-xr-x`) e data. Pastas
//! vêm antes dos arquivos; arquivos ocultos aparecem com Ctrl+H.

use std::cell::Cell;
use std::path::Path;
use std::rc::Rc;

use gtk::prelude::*;
use gtk::{gio, glib};

/// Atributos que o GIO lê para cada arquivo.
const ATTRIBUTES: &str = "standard::name,standard::display-name,standard::type,\
standard::size,standard::symbolic-icon,standard::is-hidden,standard::is-symlink,\
standard::content-type,unix::mode,owner::user,time::modified";

#[derive(Debug, Clone)]
pub struct FileList {
    pub dir_list: gtk::DirectoryList,
    pub selection: gtk::MultiSelection,
    pub view: gtk::ColumnView,
    pub scrolled: gtk::ScrolledWindow,
    filter: gtk::CustomFilter,
    show_hidden: Rc<Cell<bool>>,
}

impl FileList {
    pub fn new() -> Self {
        let dir_list = gtk::DirectoryList::new(Some(ATTRIBUTES), None::<&gio::File>);
        dir_list.set_monitored(true);

        let show_hidden = Rc::new(Cell::new(false));
        let hidden = show_hidden.clone();
        let filter = gtk::CustomFilter::new(move |obj| {
            hidden.get() || !info(obj).is_some_and(gio::FileInfo::is_hidden)
        });
        let filtered = gtk::FilterListModel::new(Some(dir_list.clone()), Some(filter.clone()));

        let view = gtk::ColumnView::builder()
            .show_column_separators(false)
            .reorderable(false)
            .build();
        view.add_css_class("data-table");

        // Pastas primeiro; depois, a coluna que o usuário escolher.
        let folders_first = gtk::CustomSorter::new(|a, b| {
            let dir = |o: &glib::Object| {
                info(o).is_some_and(|i| i.file_type() == gio::FileType::Directory)
            };
            dir(b).cmp(&dir(a)).into()
        });
        let sorter = gtk::MultiSorter::new();
        sorter.append(folders_first);
        if let Some(s) = view.sorter() {
            sorter.append(s);
        }
        let sort_model = gtk::SortListModel::new(Some(filtered), Some(sorter));
        let selection = gtk::MultiSelection::new(Some(sort_model));
        view.set_model(Some(&selection));

        let name_col = add_column(&view, &selection, "Nome", true, name_cell, Some(by_name()));
        add_column(
            &view,
            &selection,
            "Tamanho",
            false,
            |i| text_cell(&size_text(i)),
            Some(by_size()),
        );
        add_column(
            &view,
            &selection,
            "Dono",
            false,
            |i| text_cell(&i.attribute_string("owner::user").unwrap_or_default()),
            None,
        );
        add_column(
            &view,
            &selection,
            "Permissões",
            false,
            |i| {
                let l = text_cell(&msh_files::format::permissions(
                    i.attribute_uint32("unix::mode"),
                ));
                l.add_css_class("monospace");
                l
            },
            None,
        );
        add_column(
            &view,
            &selection,
            "Modificado",
            false,
            |i| text_cell(&date_text(i)),
            Some(by_date()),
        );
        view.sort_by_column(Some(&name_col), gtk::SortType::Ascending);

        let scrolled = gtk::ScrolledWindow::builder()
            .child(&view)
            .vexpand(true)
            .hexpand(true)
            .build();

        Self {
            dir_list,
            selection,
            view,
            scrolled,
            filter,
            show_hidden,
        }
    }

    pub fn set_dir(&self, path: &Path) {
        self.dir_list.set_file(Some(&gio::File::for_path(path)));
    }

    /// Relê a pasta. O GIO já acompanha as mudanças, mas o GTK 4.14 (Ubuntu
    /// 24.04) não tira o nome antigo da lista num `rename`; depois das
    /// nossas próprias ações, relemos para não depender disso.
    pub fn reload(&self) {
        if let Some(file) = self.dir_list.file() {
            self.dir_list.set_file(None::<&gio::File>);
            self.dir_list.set_file(Some(&file));
        }
    }

    pub fn toggle_hidden(&self) {
        self.show_hidden.set(!self.show_hidden.get());
        self.filter.changed(gtk::FilterChange::Different);
    }

    /// O `FileInfo` da linha `pos` (na ordem que aparece na tela).
    pub fn info_at(&self, pos: u32) -> Option<gio::FileInfo> {
        self.selection.item(pos).and_downcast()
    }

    /// Os nomes que aparecem na tela, na ordem (usado pelo autoteste).
    pub fn names(&self) -> Vec<String> {
        (0..self.selection.n_items())
            .filter_map(|i| self.info_at(i))
            .map(|i| i.name().to_string_lossy().into_owned())
            .collect()
    }

    /// Os `FileInfo` selecionados.
    pub fn selected(&self) -> Vec<gio::FileInfo> {
        let set = self.selection.selection();
        (0..set.size())
            .filter_map(|i| self.info_at(set.nth(u32::try_from(i).unwrap_or(u32::MAX))))
            .collect()
    }
}

fn info(obj: &glib::Object) -> Option<&gio::FileInfo> {
    obj.downcast_ref::<gio::FileInfo>()
}

fn add_column(
    view: &gtk::ColumnView,
    selection: &gtk::MultiSelection,
    title: &str,
    expand: bool,
    make: fn(&gio::FileInfo) -> gtk::Widget,
    sorter: Option<gtk::Sorter>,
) -> gtk::ColumnViewColumn {
    let factory = gtk::SignalListItemFactory::new();
    let selection = selection.clone();
    factory.connect_bind(move |_, item| {
        let Some(item) = item.downcast_ref::<gtk::ListItem>() else {
            return;
        };
        let widget = item
            .item()
            .and_downcast::<gio::FileInfo>()
            .map(|i| make(&i));
        if let Some(w) = &widget {
            // O botão direito seleciona a linha antes do menu de contexto
            // (que a lista abre), como nos outros gerenciadores de arquivos.
            let click = gtk::GestureClick::builder().button(3).build();
            let selection = selection.clone();
            let item = item.clone();
            click.connect_pressed(move |_, _, _, _| {
                let pos = item.position();
                if !selection.is_selected(pos) {
                    selection.select_item(pos, true);
                }
            });
            w.add_controller(click);
        }
        item.set_child(widget.as_ref());
    });
    let col = gtk::ColumnViewColumn::builder()
        .title(title)
        .factory(&factory)
        .expand(expand)
        .resizable(true)
        .build();
    if let Some(s) = sorter {
        col.set_sorter(Some(&s));
    }
    view.append_column(&col);
    col
}

fn name_cell(i: &gio::FileInfo) -> gtk::Widget {
    let row = gtk::Box::new(gtk::Orientation::Horizontal, 8);
    if let Some(icon) = i.symbolic_icon() {
        row.append(&gtk::Image::from_gicon(&icon));
    }
    let label = gtk::Label::builder()
        .label(i.display_name().as_str())
        .xalign(0.0)
        .ellipsize(gtk::pango::EllipsizeMode::Middle)
        .build();
    row.append(&label);
    row.upcast()
}

fn text_cell(text: &str) -> gtk::Widget {
    let l = gtk::Label::builder().label(text).xalign(0.0).build();
    l.add_css_class("dim-label");
    l.upcast()
}

fn size_text(i: &gio::FileInfo) -> String {
    if i.file_type() == gio::FileType::Directory {
        "—".to_owned()
    } else {
        msh_files::format::size(u64::try_from(i.size()).unwrap_or(0))
    }
}

fn date_text(i: &gio::FileInfo) -> String {
    i.modification_date_time()
        .and_then(|d| d.to_local().ok())
        .and_then(|d| d.format("%d/%m/%Y %H:%M").ok())
        .map(|s| s.to_string())
        .unwrap_or_default()
}

fn custom(
    f: impl Fn(&gio::FileInfo, &gio::FileInfo) -> std::cmp::Ordering + 'static,
) -> gtk::Sorter {
    gtk::CustomSorter::new(move |a, b| match (info(a), info(b)) {
        (Some(a), Some(b)) => f(a, b).into(),
        _ => gtk::Ordering::Equal,
    })
    .upcast()
}

fn by_name() -> gtk::Sorter {
    custom(|a, b| {
        let key = |i: &gio::FileInfo| i.display_name().to_lowercase();
        key(a).cmp(&key(b))
    })
}

fn by_size() -> gtk::Sorter {
    custom(|a, b| a.size().cmp(&b.size()))
}

fn by_date() -> gtk::Sorter {
    custom(|a, b| {
        let t = |i: &gio::FileInfo| i.attribute_uint64("time::modified");
        t(a).cmp(&t(b))
    })
}
