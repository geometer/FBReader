/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLibrary.h>
#include <ZLDialogManager.h>

#include "ZLGtkSelectionDialog.h"
#include "ZLGtkUtil.h"

static void activatedHandler(GtkTreeView *view, GtkTreePath*, GtkTreeViewColumn*) {
	((ZLGtkSelectionDialog*)gtk_object_get_user_data(GTK_OBJECT(view)))->activatedSlot();
}

static gboolean clickHandler(GtkWidget*, GdkEventButton *event, gpointer self) {
	if (event->button == 1) {
		((ZLGtkSelectionDialog*)self)->activatedSlot();
	}

	return false;
}

ZLGtkSelectionDialog::ZLGtkSelectionDialog(const char *caption, ZLTreeHandler &handler) : ZLSelectionDialog(handler) {
	myExitFlag = false;
	myNodeSelected = false;

	myDialog = createGtkDialog(caption);

	std::string okString = gtkButtonName(ZLDialogManager::OK_BUTTON);
	std::string cancelString = gtkButtonName(ZLDialogManager::CANCEL_BUTTON);
	gtk_dialog_add_button(myDialog, okString.c_str(), GTK_RESPONSE_ACCEPT);
	gtk_dialog_add_button(myDialog, cancelString.c_str(), GTK_RESPONSE_REJECT);

	myStateLine = GTK_ENTRY(gtk_entry_new());

	gtk_editable_set_editable(GTK_EDITABLE(myStateLine), !this->handler().isOpenHandler());
	gtk_widget_set_sensitive(GTK_WIDGET(myStateLine), !this->handler().isOpenHandler());

	gtk_box_pack_start(GTK_BOX(myDialog->vbox), GTK_WIDGET(myStateLine), false, false, 2);

	gtk_widget_show(GTK_WIDGET(myStateLine));

	myStore = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);
	myView = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(myStore)));

	gtk_object_set_user_data(GTK_OBJECT(myView), this);
	gtk_tree_view_set_headers_visible(myView, false);

	GtkTreeSelection *selection = gtk_tree_view_get_selection(myView);

	gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);

	// MSS: in case we do not want single click navigation, comment out the line below
	g_signal_connect(myView, "button-press-event", G_CALLBACK(clickHandler), this);

	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	column = gtk_tree_view_column_new();
	gtk_tree_view_insert_column(myView, column, -1);
	gtk_tree_view_column_set_resizable(column, true);

	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(column, renderer, false);
	gtk_tree_view_column_add_attribute(column, renderer, "pixbuf", 0);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, renderer, true);
	gtk_tree_view_column_add_attribute(column, renderer, "text", 1);

	g_signal_connect(myView, "row-activated", G_CALLBACK(activatedHandler), 0);

	GtkWidget *scrolledWindow = gtk_scrolled_window_new(0, 0);
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(myView));
	gtk_box_pack_start(GTK_BOX(myDialog->vbox), scrolledWindow, true, true, 2);
	gtk_widget_show_all(scrolledWindow);

	gtk_widget_grab_focus(GTK_WIDGET(myView));
	gtk_window_resize(GTK_WINDOW(myDialog), 1000, 1000);

	update();
}

ZLGtkSelectionDialog::~ZLGtkSelectionDialog() {
	for(std::map<std::string,GdkPixbuf*>::iterator it = myPixmaps.begin(); it != myPixmaps.end(); ++it) {
		if (it->second != 0) {
			g_object_unref(G_OBJECT(it->second));
		}
	}

	gtk_widget_destroy(GTK_WIDGET(myDialog));
}

GdkPixbuf *ZLGtkSelectionDialog::getPixmap(const ZLTreeNodePtr node) {
	const std::string &pixmapName = node->pixmapName();
	std::map<std::string,GdkPixbuf*>::const_iterator it = myPixmaps.find(pixmapName);
	if (it == myPixmaps.end()) {
		GdkPixbuf *pixmap = gdk_pixbuf_new_from_file((ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + pixmapName + ".png").c_str(), 0);
		myPixmaps[pixmapName] = pixmap;
		return pixmap;
	} else {
		return it->second;
	}
}

void ZLGtkSelectionDialog::updateStateLine() {
	gtk_entry_set_text(myStateLine, handler().stateDisplayName().c_str());
}

void ZLGtkSelectionDialog::updateList() {
	gtk_list_store_clear(myStore);

	const std::vector<ZLTreeNodePtr> &nodes = handler().subnodes();
	if (nodes.empty()) {
		return;
	}

	int index = 0;
	for (std::vector<ZLTreeNodePtr>::const_iterator it = nodes.begin(); it != nodes.end(); ++it, ++index) {
		GtkTreeIter iter;
		gtk_list_store_append(myStore, &iter);
		gtk_list_store_set(myStore, &iter,
					0, getPixmap(*it),
					1, (*it)->displayName().c_str(),
					2, index,
					-1);
	}
}

void ZLGtkSelectionDialog::selectItem(int index) {
	GtkTreeIter iter;
	if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(myStore), &iter)) {
		return;
	}
	while (index > 0) {
		if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(myStore), &iter)) {
			return;
		}
		--index;
	}
	GtkTreeSelection *selection = gtk_tree_view_get_selection(myView);
	gtk_tree_selection_select_iter(selection, &iter);
	GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(myStore), &iter);
	gtk_tree_view_scroll_to_cell(myView, path, 0, false, 0, 0);
	gtk_tree_path_free(path);
}

bool ZLGtkSelectionDialog::run() {
	while (gtk_dialog_run(myDialog) == GTK_RESPONSE_ACCEPT) {
		if (myNodeSelected || handler().isOpenHandler()) {
			GtkTreeSelection *selection = gtk_tree_view_get_selection(myView);
			GtkTreeModel *dummy;
			GtkTreeIter iter;
			if (gtk_tree_selection_get_selected(selection, &dummy, &iter)) {
				int index;
				gtk_tree_model_get(GTK_TREE_MODEL(myStore), &iter, 2, &index, -1);
				const std::vector<ZLTreeNodePtr> &nodes = handler().subnodes();
				if ((index >= 0) && (index < (int)nodes.size())) {
					runNode(nodes[index]);
				}
			}
			myNodeSelected = false;
		} else {
			runState(gtk_entry_get_text(myStateLine));	
		}
		if (myExitFlag) {
			return true;
		}
	}
	return false;
}

void ZLGtkSelectionDialog::exitDialog() {
	myExitFlag = true;
}

void ZLGtkSelectionDialog::activatedSlot() {
	myNodeSelected = true;
	gtk_dialog_response(myDialog, GTK_RESPONSE_ACCEPT);
}
