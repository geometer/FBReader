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

#include <gtk/gtklabel.h>
#include <gtk/gtkbox.h>

#include <ZLDialogManager.h>

#include "ZLGtkOptionsDialog.h"
#include "ZLGtkDialogContent.h"
#include "ZLGtkUtil.h"

ZLGtkOptionsDialog::ZLGtkOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) : ZLDesktopOptionsDialog(resource, applyAction) {
	myDialog = createGtkDialog(caption());

	std::string okString = gtkString(ZLDialogManager::buttonName(ZLDialogManager::OK_BUTTON));
	std::string cancelString = gtkString(ZLDialogManager::buttonName(ZLDialogManager::CANCEL_BUTTON));
	gtk_dialog_add_button(myDialog, okString.c_str(), GTK_RESPONSE_ACCEPT);
	gtk_dialog_add_button(myDialog, cancelString.c_str(), GTK_RESPONSE_REJECT);
	if (showApplyButton) {
		std::string applyString = gtkString(ZLDialogManager::buttonName(ZLDialogManager::APPLY_BUTTON));
		gtk_dialog_add_button(myDialog, applyString.c_str(), GTK_RESPONSE_APPLY);
	}

	myNotebook = GTK_NOTEBOOK(gtk_notebook_new());
	gtk_notebook_set_scrollable(myNotebook, true);

	gtk_container_set_border_width(GTK_CONTAINER(myNotebook), 8);
	gtk_box_pack_start(GTK_BOX(myDialog->vbox), GTK_WIDGET(myNotebook), TRUE, TRUE, 0);

	gtk_widget_show(GTK_WIDGET(myNotebook));
}

ZLGtkOptionsDialog::~ZLGtkOptionsDialog() {
	// I do not have to destroy myNotebook as it's a myDialog child
	destroyGtkDialog(myDialog);
}

ZLDialogContent &ZLGtkOptionsDialog::createTab(const ZLResourceKey &key) {
	ZLGtkDialogContent *tab = new ZLGtkDialogContent(tabResource(key));

	gtk_notebook_append_page(myNotebook, tab->widget(), gtk_label_new(tab->displayName().c_str()));

	myTabs.push_back(tab);

	return *tab;
}

const std::string &ZLGtkOptionsDialog::selectedTabKey() const {
	return myTabs[gtk_notebook_get_current_page(myNotebook)]->key();
}

void ZLGtkOptionsDialog::selectTab(const ZLResourceKey &key) {
	std::vector<shared_ptr<ZLDialogContent> >::const_iterator it;
	for (it = myTabs.begin(); it != myTabs.end(); ++it) {
		if ((*it)->key() == key.Name) {
			break;
		}
	}
	if (it != myTabs.end()) {
		gtk_notebook_set_current_page(myNotebook, it - myTabs.begin());
	}
}

bool ZLGtkOptionsDialog::runInternal() {
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
