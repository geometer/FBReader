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
#include <gtk/gtkscrolledwindow.h>

#include <ZLDialogManager.h>

#include "ZLGtkOptionsDialog.h"
#include "ZLGtkDialogContent.h"
#include "ZLGtkUtil.h"

ZLGtkOptionsDialog::ZLGtkOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction) : ZLOptionsDialog(resource, applyAction) {
	myDialog = createGtkDialog(caption());

	std::string okString = gtkButtonName(ZLDialogManager::OK_BUTTON);
	std::string cancelString = gtkButtonName(ZLDialogManager::CANCEL_BUTTON);
	gtk_dialog_add_button(myDialog, okString.c_str(), GTK_RESPONSE_ACCEPT);
	gtk_dialog_add_button(myDialog, cancelString.c_str(), GTK_RESPONSE_REJECT);

	myNotebook = GTK_NOTEBOOK(gtk_notebook_new());
	gtk_notebook_set_scrollable(myNotebook, true);

	gtk_container_set_border_width(GTK_CONTAINER(myNotebook), 8);
	gtk_box_pack_start(GTK_BOX(myDialog->vbox), GTK_WIDGET(myNotebook), true, true, 0);

	gtk_widget_show(GTK_WIDGET(myNotebook));
	gtk_window_resize(GTK_WINDOW(myDialog), 1000, 1000);
}

ZLGtkOptionsDialog::~ZLGtkOptionsDialog() {
	// I do not have to destroy myNotebook as it's a myDialog child
	gtk_widget_destroy(GTK_WIDGET(myDialog));
}

ZLDialogContent &ZLGtkOptionsDialog::createTab(const ZLResourceKey &key) {
	ZLGtkDialogContent *tab = new ZLGtkDialogContent(tabResource(key));
	GtkWidget *label = gtk_label_new(tab->displayName().c_str());

	GtkScrolledWindow *scrolledWindow = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(0, 0));
	gtk_scrolled_window_set_policy(scrolledWindow, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_add_with_viewport(scrolledWindow, tab->widget());
	gtk_widget_show(GTK_WIDGET(scrolledWindow));
	gtk_notebook_append_page(myNotebook, GTK_WIDGET(scrolledWindow), label);

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
	bool code = gtk_dialog_run(myDialog) == GTK_RESPONSE_ACCEPT;
	gtk_widget_hide(GTK_WIDGET(myDialog));
	return code;
}
