/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLApplication.h>

#include "ZLWin32SelectionDialog.h"

#include "../w32widgets/W32Container.h"
#include "../w32widgets/W32Control.h"
#include "../w32widgets/W32TreeView.h"

#include "../application/ZLWin32ApplicationWindow.h"

/*
static void activatedHandler(Win32TreeView *view, Win32TreePath*, Win32TreeViewColumn*) {
	((ZLWin32SelectionDialog*)gtk_object_get_user_data(GTK_OBJECT(view)))->activatedSlot();
}

static gboolean clickHandler(Win32Widget*, GdkEventButton *event, gpointer self) {
	if (event->button == 1) {
		((ZLWin32SelectionDialog*)self)->activatedSlot();
	}

	return false;
}
*/

ZLWin32SelectionDialog::ZLWin32SelectionDialog(ZLWin32ApplicationWindow &window, const std::string &caption, ZLTreeHandler &handler) : ZLDesktopSelectionDialog(handler), myWindow(window), myPanel(myWindow.mainWindow(), caption) {
	W32VBox *panelBox = new W32VBox();
	myPanel.setElement(panelBox);

	W32TreeView *treeView = new W32TreeView();
	treeView->setVisible(true);
	panelBox->addElement(treeView);
	/*
	ZLWin32DialogContent *contentTab = new ZLWin32DialogContent();
	myTab = contentTab;
	panelBox->addElement(contentTab->contentPtr());
	W32Table &table = contentTab->contentTable();
	*/

	const short charHeight = myPanel.charDimension().Height;
	panelBox->setSpacing(charHeight / 2);
	panelBox->setMargins(charHeight / 2, charHeight / 2, charHeight / 2, charHeight / 2);
	//table.setSpacings(charHeight / 2, charHeight);
	//table.setMargins(charHeight / 2, charHeight / 2, charHeight / 2, charHeight / 2);

	W32HBox *buttonBox = new W32HBox();
	panelBox->addElement(buttonBox);
	buttonBox->setHomogeneous(true);
	buttonBox->setSpacing(charHeight / 2);
	buttonBox->setMargins(charHeight / 2, charHeight / 2, charHeight / 2, charHeight / 2);
	buttonBox->addElement(new W32PushButton("&Ok", W32PushButton::OK_BUTTON));
	buttonBox->addElement(new W32PushButton("&Cancel", W32PushButton::CANCEL_BUTTON));
	buttonBox->setVisible(true);
/*
	myExitFlag = false;
	myNodeSelected = false;

	myDialog = createWin32Dialog(caption);

	std::string okString = gtkString("&Ok");
	std::string cancelString = gtkString("&Cancel");
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

	Win32TreeSelection *selection = gtk_tree_view_get_selection(myView);

	gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);

	// MSS: in case we do not want single click navigation, comment out the line below
	g_signal_connect(myView, "button-press-event", G_CALLBACK(clickHandler), this);

	Win32CellRenderer *renderer;
	Win32TreeViewColumn *column;

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

	Win32Widget *scrolledWindow = gtk_scrolled_window_new(0, 0);
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(myView));
	gtk_box_pack_start(GTK_BOX(myDialog->vbox), scrolledWindow, true, true, 2);
	gtk_widget_show_all(scrolledWindow);

	gtk_widget_grab_focus(GTK_WIDGET(myView));

	update();
*/
}

ZLWin32SelectionDialog::~ZLWin32SelectionDialog() {
/*
	for(std::map<std::string,GdkPixbuf*>::iterator it = myPixmaps.begin(); it != myPixmaps.end(); ++it) {
		if (it->second != 0) {
			g_object_unref(G_OBJECT(it->second));
		}
	}

	destroyWin32Dialog(myDialog);
*/
}

/*
GdkPixbuf *ZLWin32SelectionDialog::getPixmap(const ZLTreeNodePtr node) {
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
*/

void ZLWin32SelectionDialog::updateStateLine() {
	//gtk_entry_set_text(myStateLine, handler().stateDisplayName().c_str());
}

void ZLWin32SelectionDialog::updateList() {
/*
	gtk_list_store_clear(myStore);

	const std::vector<ZLTreeNodePtr> &nodes = handler().subnodes();
	if (nodes.empty()) {
		return;
	}

	int index = 0;
	for (std::vector<ZLTreeNodePtr>::const_iterator it = nodes.begin(); it != nodes.end(); ++it, ++index) {
		Win32TreeIter iter;
		gtk_list_store_append(myStore, &iter);
		gtk_list_store_set(myStore, &iter,
					0, getPixmap(*it),
					1, (*it)->displayName().c_str(),
					2, index,
					-1);
	}
*/
}

void ZLWin32SelectionDialog::selectItem(int index) {
/*
	Win32TreeIter iter;
	if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(myStore), &iter)) {
		return;
	}
	while (index > 0) {
		if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(myStore), &iter)) {
			return;
		}
		--index;
	}
	Win32TreeSelection *selection = gtk_tree_view_get_selection(myView);
	gtk_tree_selection_select_iter(selection, &iter);
	Win32TreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(myStore), &iter);
	gtk_tree_view_scroll_to_cell(myView, path, 0, false, 0, 0);
	gtk_tree_path_free(path);
*/
}

bool ZLWin32SelectionDialog::run() {
	myWindow.blockMouseEvents(true);
	bool result = myPanel.runDialog();
	myWindow.blockMouseEvents(false);
	return false;

/*
	while (gtk_dialog_run(myDialog) == GTK_RESPONSE_ACCEPT) {
		if (myNodeSelected || handler().isOpenHandler()) {
			Win32TreeSelection *selection = gtk_tree_view_get_selection(myView);
			Win32TreeModel *dummy;
			Win32TreeIter iter;

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
*/
}

void ZLWin32SelectionDialog::exitDialog() {
	//myExitFlag = true;
}

/*
void ZLWin32SelectionDialog::activatedSlot() {
	myNodeSelected = true;
	gtk_dialog_response(myDialog, GTK_RESPONSE_ACCEPT);
}
*/

void ZLWin32SelectionDialog::setSize(int width, int height) {
	//gtk_window_resize(GTK_WINDOW(myDialog), width, height);
}

int ZLWin32SelectionDialog::width() const {
	/*
	int _width;
	gtk_window_get_size(GTK_WINDOW(myDialog), &_width, 0);
	return _width;
	*/
	return 200;
}

int ZLWin32SelectionDialog::height() const {
	/*
	int _height;
	gtk_window_get_size(GTK_WINDOW(myDialog), 0, &_height);
	return _height;
	*/
	return 200;
}
