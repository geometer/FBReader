/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "GtkOpenFileDialog.h"
#include "GtkDialogManager.h"

static bool dialogDefaultKeys(GtkWidget *dialog, GdkEventKey *key, gpointer) {
	if (key->keyval == GDK_Return && key->state == 0) {
		gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
		return true;
	} else if (key->keyval == GDK_Escape && key->state == 0) {
		gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_REJECT);
		return true;
	}

	return false;
}

static void activatedHandler(GtkTreeView *view, GtkTreePath *, GtkTreeViewColumn *) {
	((GtkOpenFileDialog *)gtk_object_get_user_data(GTK_OBJECT(view)))->activatedSlot();
}

GtkOpenFileDialog::GtkOpenFileDialog(const char *caption, const ZLTreeHandler &handler) : ZLDesktopOpenFileDialog(handler) {
	myExitFlag = false;
	myDialog = GTK_DIALOG(gtk_dialog_new_with_buttons(caption, 0, GTK_DIALOG_MODAL,
					GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
					GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
					0));

	gtk_signal_connect(GTK_OBJECT(myDialog), "key_press_event", G_CALLBACK(dialogDefaultKeys), 0);

	myCurrentDirectoryName = GTK_ENTRY(gtk_entry_new());

	gtk_editable_set_editable(GTK_EDITABLE(myCurrentDirectoryName), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(myCurrentDirectoryName), FALSE);

	gtk_box_pack_start(GTK_BOX(myDialog->vbox), GTK_WIDGET(myCurrentDirectoryName), FALSE, FALSE, 2);

	gtk_widget_show(GTK_WIDGET(myCurrentDirectoryName));

	myStore = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);
	myView = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(myStore)));

	gtk_object_set_user_data(GTK_OBJECT(myView), this);
	gtk_tree_view_set_headers_visible(myView, FALSE);

	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	column = gtk_tree_view_column_new();
	gtk_tree_view_insert_column(myView, column, -1);
	gtk_tree_view_column_set_resizable(column, TRUE);

	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(column, renderer, FALSE);
	gtk_tree_view_column_add_attribute(column, renderer, "pixbuf", 0);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_add_attribute(column, renderer, "text", 1);

	g_signal_connect(myView, "row-activated", G_CALLBACK(activatedHandler), 0);

	GtkWidget *scrolledWindow = gtk_scrolled_window_new(0, 0);
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(myView));
	gtk_box_pack_start(GTK_BOX(myDialog->vbox), scrolledWindow, TRUE, TRUE, 2);
	gtk_widget_show_all(scrolledWindow);

	update("");

	gtk_widget_grab_focus(GTK_WIDGET(myView));
}

GtkOpenFileDialog::~GtkOpenFileDialog(void) {
	for(std::map<std::string,GdkPixbuf*>::iterator it = myPixmaps.begin(); it != myPixmaps.end(); ++it) {
		g_object_unref(G_OBJECT(it->second));
	}

	gtk_widget_destroy(GTK_WIDGET(myDialog));
}

GdkPixbuf *GtkOpenFileDialog::getPixmap(const ZLTreeNodePtr node) {
	std::string pName = pixmapName(node);
	std::map<std::string,GdkPixbuf*>::const_iterator it = myPixmaps.find(pName);
	if (it == myPixmaps.end()) {
		GdkPixbuf *pixmap = gdk_pixbuf_new_from_file((((GtkDialogManager&)GtkDialogManager::instance()).getPixmapPath() + "/" + pName + ".png").c_str(), 0);
		myPixmaps[pName] = pixmap;
		return pixmap;
	} else {
		return it->second;
	}
}

void GtkOpenFileDialog::update(const std::string &selectedNodeName) {
	gtk_entry_set_text(myCurrentDirectoryName, myCurrentDir->name().c_str());

	gtk_list_store_clear(myStore);
	myNodes.clear();

	GtkTreeIter *selectedItem = 0;

	const std::vector<ZLTreeNodePtr> &subnodes = myCurrentDir->subnodes();
	int index = 0;
	for (std::vector<ZLTreeNodePtr>::const_iterator it = subnodes.begin(); it != subnodes.end(); it++, index++) {
		GtkTreeIter iter;
		gtk_list_store_append(myStore, &iter);

		gtk_list_store_set(myStore, &iter,
					0, getPixmap(*it),
					1, (*it)->name().c_str(),
					2, index,
					-1);

		myNodes.push_back(*it);

		if ((*it)->name() == selectedNodeName) {
			selectedItem = gtk_tree_iter_copy(&iter);
		}
	}

	GtkTreeSelection *selection = gtk_tree_view_get_selection(myView);

	if (selectedItem == 0) {
		GtkTreeIter iter;
		gtk_tree_model_get_iter_first(GTK_TREE_MODEL(myStore), &iter);
		gtk_tree_selection_select_iter(selection, &iter);
	} else {
		gtk_tree_selection_select_iter(selection, selectedItem);
		GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(myStore), selectedItem);
		gtk_tree_view_scroll_to_cell(myView, path, 0, FALSE, 0, 0);
		gtk_tree_path_free(path);
		gtk_tree_iter_free(selectedItem);
	}
}

void GtkOpenFileDialog::run() {
	while (gtk_dialog_run(myDialog) == GTK_RESPONSE_ACCEPT) {
		GtkTreeSelection *selection = gtk_tree_view_get_selection(myView);
		GtkTreeModel *dummy;
		GtkTreeIter iter;

		if (gtk_tree_selection_get_selected(selection, &dummy, &iter)) {
			int index;
			gtk_tree_model_get(GTK_TREE_MODEL(myStore), &iter, 2, &index, -1);
			runNode(myNodes[index]);
			if (myExitFlag) {
				break;
			}
		}
	}
}

void GtkOpenFileDialog::exitDialog() {
	myExitFlag = true;
}

void GtkOpenFileDialog::activatedSlot() {
	gtk_dialog_response(myDialog, GTK_RESPONSE_ACCEPT);
}

void GtkOpenFileDialog::setSize(int width, int height) {
	gtk_window_resize(GTK_WINDOW(myDialog), width, height);
}

int GtkOpenFileDialog::width() const {
	int _width;
	gtk_window_get_size(GTK_WINDOW(myDialog), &_width, 0);
	return _width;
}

int GtkOpenFileDialog::height() const {
	int _height;
	gtk_window_get_size(GTK_WINDOW(myDialog), 0, &_height);
	return _height;
}

// vim:ts=2:sw=2:noet
