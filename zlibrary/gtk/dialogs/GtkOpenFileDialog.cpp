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

#include <iostream>

#include "../../abstract/filesystem/ZLFSManager.h"
#include "../../abstract/filesystem/ZLFSDir.h"
#include "../../abstract/filesystem/ZLZipDir.h"
#include "../../abstract/util/ZLStringUtil.h"

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

static void
activatedHandler(GtkTreeView *view, GtkTreePath *path, GtkTreeViewColumn *column) {
	((GtkOpenFileDialog *)gtk_object_get_user_data(GTK_OBJECT(view)))->activatedSlot(path, column);
}

GtkOpenFileDialog::GtkOpenFileDialog(const char *caption, const ZLFileHandler &handler) : ZLOpenFileDialog(handler) {
	myDialog = GTK_DIALOG(gtk_dialog_new_with_buttons(caption, NULL, GTK_DIALOG_MODAL,
					GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
					GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
					NULL));

	gtk_signal_connect(GTK_OBJECT(myDialog), "key_press_event", G_CALLBACK(dialogDefaultKeys), NULL);

	gtk_window_set_default_size(GTK_WINDOW(myDialog), 600, 600);

	myCurrentDirectoryName = GTK_ENTRY(gtk_entry_new());

	gtk_editable_set_editable(GTK_EDITABLE(myCurrentDirectoryName), FALSE);

	gtk_entry_set_text(myCurrentDirectoryName, myCurrentDir->name().c_str());

	gtk_box_pack_start(GTK_BOX(myDialog->vbox), GTK_WIDGET(myCurrentDirectoryName), FALSE, FALSE, 2);

	gtk_widget_show(GTK_WIDGET(myCurrentDirectoryName));

	myStore = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_BOOLEAN);
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

	g_signal_connect(myView, "row-activated", G_CALLBACK(activatedHandler), NULL);

	GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);

	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(myView));

	gtk_box_pack_start(GTK_BOX(myDialog->vbox), scrolledWindow, TRUE, TRUE, 2);

	gtk_widget_show_all(scrolledWindow);

	updateListView("..");
}

GtkOpenFileDialog::~GtkOpenFileDialog(void) {
	for(std::map<std::string,GdkPixbuf*>::iterator it = myPixmaps.begin(); it != myPixmaps.end(); ++it) {
		g_object_unref(G_OBJECT(it->second));
	}

	gtk_widget_destroy(GTK_WIDGET(myDialog));
}

GdkPixbuf *GtkOpenFileDialog::getPixmap(const std::string &fileName, bool dir) {
	std::string pixmapName = handler().pixmapName(fileName, dir);
	std::map<std::string,GdkPixbuf*>::const_iterator it = myPixmaps.find(pixmapName);
	if (it == myPixmaps.end()) {
		GdkPixbuf *pixmap = gdk_pixbuf_new_from_file((((GtkDialogManager&)GtkDialogManager::instance()).getPixmapPath() + "/" + pixmapName + ".png").c_str(), NULL);
		myPixmaps[pixmapName] = pixmap;
		return pixmap;
	} else {
		return it->second;
	}
}

void GtkOpenFileDialog::updateListView(const std::string &selected) {
	gtk_list_store_clear(myStore);

	GtkTreeIter iter;
	GtkTreeIter *selectedItem = 0;

	if ((myCurrentDir->name() != "/") && (handler().isFileVisible("..", true))) {
		gtk_list_store_append(myStore, &iter);

		gtk_list_store_set(myStore, &iter,
					0, getPixmap("..", true),
					1, "..",
					2, true,
					-1);

		if (selected == "..") {
			selectedItem = gtk_tree_iter_copy(&iter);
		}
	}

	std::vector<std::string> dirNames;
	myCurrentDir->collectSubDirs(dirNames, true);
	for (std::vector<std::string>::const_iterator it = dirNames.begin(); it != dirNames.end(); it++) {
		if (handler().isFileVisible(*it, true)) {
			gtk_list_store_append(myStore, &iter);

			gtk_list_store_set(myStore, &iter,
						0, getPixmap(it->c_str(), true),
						1, it->c_str(),
						2, true,
						-1);

			if (*it == selected) {
				selectedItem = gtk_tree_iter_copy(&iter);
			}
		}
	}

	std::vector<std::string> fileNames;
	myCurrentDir->collectFiles(fileNames, true);
	for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); it++) {
		if (handler().isFileVisible(*it, false)) {
			gtk_list_store_append(myStore, &iter);

			gtk_list_store_set(myStore, &iter,
						0, getPixmap(it->c_str(), false),
						1, it->c_str(),
						2, false,
						-1);

			if (*it == selected) {
				selectedItem = gtk_tree_iter_copy(&iter);
			}
		}
	}

	GtkTreeSelection *selection = gtk_tree_view_get_selection(myView);

	if (selectedItem == 0) {
		gtk_tree_model_get_iter_first(GTK_TREE_MODEL(myStore), &iter);

		gtk_tree_selection_select_iter(selection, selectedItem);
	} else {
		gtk_tree_selection_select_iter(selection, selectedItem);

		GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(myStore), selectedItem);

		gtk_tree_view_scroll_to_cell(myView, path, NULL, FALSE, 0, 0);

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
			char *name;
			gboolean isDir;

			gtk_tree_model_get(GTK_TREE_MODEL(myStore), &iter, 1, &name, 2, &isDir, -1);  // extract file name

			if (isDir) {
				std::string subdir = myCurrentDir->itemName(name);
				std::string selectedName = (std::string(name) == "..") ? myCurrentDir->shortName() : "..";
				delete myCurrentDir;
				myCurrentDir = ZLFSManager::instance().createDirectory(subdir);
				gtk_entry_set_text(myCurrentDirectoryName, myCurrentDir->name().c_str());
				updateListView(selectedName);
			} else if (ZLStringUtil::stringEndsWith(name, ".zip")) {
				std::string zip = myCurrentDir->itemName(name);
				delete myCurrentDir;
				myCurrentDir = new ZLZipDir(zip);
				gtk_entry_set_text(myCurrentDirectoryName, myCurrentDir->name().c_str());
				updateListView("..");
			} else {
				handler().accept(myCurrentDir->itemName(name), isDir);

				// MSS: ugly piece of code :(
				g_free(name);
				break;
			}

			g_free(name);
		}
	}
}

void
GtkOpenFileDialog::activatedSlot(GtkTreePath *path, GtkTreeViewColumn *column) {
	gtk_dialog_response(myDialog, GTK_RESPONSE_ACCEPT);
}

// vim:ts=2:sw=2:noet
