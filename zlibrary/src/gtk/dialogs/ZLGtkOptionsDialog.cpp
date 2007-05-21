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

#include "ZLGtkOptionsDialog.h"
#include "ZLGtkDialogContent.h"
#include "ZLGtkUtil.h"

ZLGtkOptionsDialog::ZLGtkOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) : ZLDesktopOptionsDialog(resource, applyAction) {
	myDialog = createGtkDialog(caption());

	std::string okString = gtkString("&Ok");
	std::string cancelString = gtkString("&Cancel");
	gtk_dialog_add_button(myDialog, okString.c_str(), GTK_RESPONSE_ACCEPT);
	gtk_dialog_add_button(myDialog, cancelString.c_str(), GTK_RESPONSE_REJECT);
	if (showApplyButton) {
		std::string applyString = gtkString("&Apply");
		gtk_dialog_add_button(myDialog, applyString.c_str(), GTK_RESPONSE_APPLY);
	}

	myNotebook = GTK_NOTEBOOK(gtk_notebook_new());

	gtk_container_set_border_width(GTK_CONTAINER(myNotebook), 8);
	gtk_box_pack_start(GTK_BOX(myDialog->vbox), GTK_WIDGET(myNotebook), TRUE, TRUE, 0);

	gtk_widget_show(GTK_WIDGET(myNotebook));
}

ZLGtkOptionsDialog::~ZLGtkOptionsDialog() {
	// I do not have to destroy myNotebook as it's a myDialog child
	destroyGtkDialog(myDialog);
}

ZLDialogContent &ZLGtkOptionsDialog::createTab(const std::string &name) {
	ZLGtkDialogContent *tab = new ZLGtkDialogContent();
	GtkWidget *label = gtk_label_new(name.c_str());

	gtk_notebook_append_page(myNotebook, tab->widget(), label);

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
	int code;
	while ((code = gtk_dialog_run(myDialog)) == GTK_RESPONSE_APPLY) {
		accept();
	}
	gtk_widget_hide(GTK_WIDGET(myDialog));
	return code == GTK_RESPONSE_ACCEPT;
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
