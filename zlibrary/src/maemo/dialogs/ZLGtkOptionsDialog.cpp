/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <gtk/gtklabel.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkscrolledwindow.h>

#include "ZLGtkOptionsDialog.h"
#include "ZLGtkDialogContent.h"
#include "ZLGtkUtil.h"

ZLGtkOptionsDialog::ZLGtkOptionsDialog(const std::string &id, const std::string &caption, GtkWindow *parent, shared_ptr<ZLRunnable> applyAction) : ZLOptionsDialog(id, applyAction) {
	myDialog = createGtkDialog(caption);
	gtk_widget_set_size_request(GTK_WIDGET(myDialog), 800, 800);

	std::string okString = gtkString("&Ok");
	std::string cancelString = gtkString("&Cancel");
	gtk_dialog_add_button(myDialog, okString.c_str(), GTK_RESPONSE_ACCEPT);
	gtk_dialog_add_button(myDialog, cancelString.c_str(), GTK_RESPONSE_REJECT);

	myNotebook = GTK_NOTEBOOK(gtk_notebook_new());

	gtk_container_set_border_width(GTK_CONTAINER(myNotebook), 8);
	gtk_box_pack_start(GTK_BOX(myDialog->vbox), GTK_WIDGET(myNotebook), TRUE, TRUE, 0);

	gtk_widget_show(GTK_WIDGET(myNotebook));
}

ZLGtkOptionsDialog::~ZLGtkOptionsDialog() {
	gtk_widget_destroy(GTK_WIDGET(myDialog));
}

ZLDialogContent &ZLGtkOptionsDialog::createTab(const std::string &name) {
	ZLGtkDialogContent *tab = new ZLGtkDialogContent();
	GtkWidget *label = gtk_label_new(name.c_str());

	GtkScrolledWindow *scrolledWindow = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(0, 0));
	gtk_scrolled_window_set_policy(scrolledWindow, GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_add_with_viewport(scrolledWindow, tab->widget());
	gtk_widget_show(GTK_WIDGET(scrolledWindow));
	gtk_notebook_append_page(myNotebook, GTK_WIDGET(scrolledWindow), label);

	myTabs.push_back(tab);
	myTabNames.push_back(name);

	return *tab;
}

const std::string &ZLGtkOptionsDialog::selectedTabName() const {
	return myTabNames[gtk_notebook_get_current_page(myNotebook)];
}

void ZLGtkOptionsDialog::selectTab(const std::string &name) {
	std::vector<std::string>::const_iterator it = std::find(myTabNames.begin(), myTabNames.end(), name);
	if (it != myTabNames.end()) {
		gtk_notebook_set_current_page(myNotebook, it - myTabNames.begin());
	}
}

bool ZLGtkOptionsDialog::run() {
	bool code = gtk_dialog_run(myDialog) == GTK_RESPONSE_ACCEPT;
	gtk_widget_hide(GTK_WIDGET(myDialog));
	return code;
}

void ZLGtkOptionsDialog::setSize(int width, int height) {
	gtk_window_resize(GTK_WINDOW(myDialog), width, height);
}

int ZLGtkOptionsDialog::width() const {
	int _width, _height;

	gtk_window_get_size(GTK_WINDOW(myDialog), &_width, &_height);

	return _width;
}

int ZLGtkOptionsDialog::height() const {
	int _width, _height;

	gtk_window_get_size(GTK_WINDOW(myDialog), &_width, &_height);

	return _height;
}
