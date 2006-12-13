/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <ZLApplication.h>

#include "GtkOpenFileDialog.h"
#include "GtkDialogManager.h"
#include "GtkUtil.h"

static void activatedHandler(GtkTreeView *view, GtkTreePath *, GtkTreeViewColumn *) {
	((GtkOpenFileDialog *)gtk_object_get_user_data(GTK_OBJECT(view)))->activatedSlot();
}

static gboolean clickHandler(GtkWidget *, GdkEventButton *event, gpointer self) {
	if (event->button == 1) {
		((GtkOpenFileDialog *)self)->activatedSlot();
	}

	return FALSE;
}

GtkOpenFileDialog::GtkOpenFileDialog(const char *caption, const ZLTreeHandler &handler) : ZLOpenFileDialog(handler) {
	myExitFlag = false;

	myDialog = createGtkDialog(caption);
	gtk_widget_set_size_request(GTK_WIDGET(myDialog), 800, 800);

	std::string okString = gtkString("&Ok");
	std::string cancelString = gtkString("&Cancel");
	gtk_dialog_add_button(myDialog, okString.c_str(), GTK_RESPONSE_ACCEPT);
	gtk_dialog_add_button(myDialog, cancelString.c_str(), GTK_RESPONSE_REJECT);

	myStateLine = GTK_ENTRY(gtk_entry_new());

	gtk_editable_set_editable(GTK_EDITABLE(myStateLine), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(myStateLine), FALSE);

	gtk_box_pack_start(GTK_BOX(myDialog->vbox), GTK_WIDGET(myStateLine), FALSE, FALSE, 2);

	gtk_widget_show(GTK_WIDGET(myStateLine));

	myStore = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);
	myView = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(myStore)));

	gtk_object_set_user_data(GTK_OBJECT(myView), this);
	gtk_tree_view_set_headers_visible(myView, FALSE);

	GtkTreeSelection *selection = gtk_tree_view_get_selection(myView);

	gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);

	// MSS: in case we do not want single click navigation, comment out the line below
	g_signal_connect(myView, "button-press-event", G_CALLBACK(clickHandler), this);

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
	const std::string &pixmapName = node->pixmapName();
	std::map<std::string,GdkPixbuf*>::const_iterator it = myPixmaps.find(pixmapName);
	if (it == myPixmaps.end()) {
		GdkPixbuf *pixmap = gdk_pixbuf_new_from_file((ZLApplication::ImageDirectory() + ZLApplication::PathDelimiter + pixmapName + ".png").c_str(), 0);
		myPixmaps[pixmapName] = pixmap;
		return pixmap;
	} else {
		return it->second;
	}
}

void GtkOpenFileDialog::update(const std::string &selectedNodeName) {
	char *stateText = g_locale_to_utf8(state()->name().data(), state()->name().length(), 0, 0, 0);
	gtk_entry_set_text(myStateLine, stateText);
	g_free(stateText);

	gtk_list_store_clear(myStore);
	myNodes.clear();

	GtkTreeIter *selectedItem = 0;

	const std::vector<ZLTreeNodePtr> &subnodes = state()->subnodes();
	int index = 0;
	for (std::vector<ZLTreeNodePtr>::const_iterator it = subnodes.begin(); it != subnodes.end(); ++it, ++index) {
		GtkTreeIter iter;
		gtk_list_store_append(myStore, &iter);

		char *fileName = g_locale_to_utf8((*it)->name().data(), (*it)->name().length(), 0, 0, 0);
		gtk_list_store_set(myStore, &iter,
					0, getPixmap(*it),
					1, fileName,
					2, index,
					-1);
		g_free(fileName);

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
