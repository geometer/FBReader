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

#include <algorithm>

#include <gtk/gtkstock.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkbox.h>
#include <gtk/gtksignal.h>

#include <gdk/gdkkeysyms.h>

#include "../../abstract/dialogs/ZLOptionEntry.h"

#include "ZLGtkOptionsDialog.h"
#include "ZLGtkOptionView.h"
#include "ZLGtkUtil.h"

ZLGtkOptionsDialog::ZLGtkOptionsDialog(const std::string &id, const std::string &caption) : ZLOptionsDialog(id) {
	myDialog = createGtkDialog(caption);

	std::string okString = gtkString("&Ok");
	std::string cancelString = gtkString("&Cancel");
	gtk_dialog_add_button (myDialog, okString.c_str(), GTK_RESPONSE_ACCEPT);
	gtk_dialog_add_button (myDialog, cancelString.c_str(), GTK_RESPONSE_REJECT);

	myNotebook = GTK_NOTEBOOK(gtk_notebook_new());
	gtk_notebook_set_scrollable(myNotebook, true);

	gtk_container_set_border_width(GTK_CONTAINER(myNotebook), 8);
	gtk_box_pack_start(GTK_BOX(myDialog->vbox), GTK_WIDGET(myNotebook), true, true, 0);

	gtk_widget_show(GTK_WIDGET(myNotebook));
	gtk_window_resize(GTK_WINDOW(myDialog), 1000, 1000);
}

ZLGtkOptionsDialog::~ZLGtkOptionsDialog() {
	// I do not have to destroy myNotebook as it's a myDialog child
	for (std::vector<ZLGtkOptionsDialogTab*>::iterator tab = myTabs.begin(); tab != myTabs.end(); ++tab) {
		delete *tab;
	}

	gtk_widget_destroy(GTK_WIDGET(myDialog));
}

ZLDialogContent &ZLGtkOptionsDialog::createTab(const std::string &name) {
	ZLGtkOptionsDialogTab *tab = new ZLGtkOptionsDialogTab();
	GtkWidget *label = gtk_label_new(name.c_str());

	GtkScrolledWindow *scrolledWindow = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(0, 0));
	gtk_scrolled_window_set_policy(scrolledWindow, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
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
	gint response = gtk_dialog_run(myDialog);

	switch (response) {
		case GTK_RESPONSE_ACCEPT:
			for (std::vector<ZLGtkOptionsDialogTab *>::iterator tab = myTabs.begin(); tab != myTabs.end(); ++tab)
				(*tab)->accept();
			break;
		case GTK_RESPONSE_REJECT:
			break;
	}

	gtk_widget_hide(GTK_WIDGET(myDialog));

	return response == GTK_RESPONSE_ACCEPT;
}

void ZLGtkOptionsDialogTab::accept() {
	for (std::vector<ZLGtkOptionView *>::iterator view = myViews.begin(); view != myViews.end(); ++view) {
		(*view)->onAccept();
	}
}

ZLGtkOptionsDialogTab::ZLGtkOptionsDialogTab() {
	myRowCounter = 0;
	myTable = GTK_TABLE(gtk_table_new(0, 12, false));
	gtk_widget_show_all(GTK_WIDGET(myTable));
}

ZLGtkOptionsDialogTab::~ZLGtkOptionsDialogTab() {
	// We must not delete the widget, it's destroyed when the parent widget is destroyed
	for (std::vector<ZLGtkOptionView *>::iterator view = myViews.begin(); view != myViews.end(); ++view) {
		delete *view;
	}
}

int ZLGtkOptionsDialogTab::addRow() {
	int row = myRowCounter++;

	gtk_table_resize(myTable, myRowCounter, 2);

	return row;
}

void ZLGtkOptionsDialogTab::addItem(GtkWidget *what, int row, int fromColumn, int toColumn) {
	gtk_table_attach(myTable, what, fromColumn, toColumn, row, row + 1, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_FILL, 2, 1);
}

void ZLGtkOptionsDialogTab::addOption(ZLOptionEntry *option) {
	int row = addRow();

	createViewByEntry(option, row, 0, 12);
}

void ZLGtkOptionsDialogTab::addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1) {
	int row = addRow();

	createViewByEntry(option0, row, 0, 6);
	createViewByEntry(option1, row, 6, 12);
}

void ZLGtkOptionsDialogTab::createViewByEntry(ZLOptionEntry *option, int row, int fromColumn, int toColumn) {
	if (option == 0) {
		return;
	}

	ZLGtkOptionView *view = 0;

	switch (option->kind()) {
		case BOOLEAN:
			view = new BooleanOptionView((ZLBooleanOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
		case STRING:
			view = new StringOptionView((ZLStringOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
		case CHOICE:
			view = new ChoiceOptionView((ZLChoiceOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
		case SPIN:
			view = new SpinOptionView((ZLSpinOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
		case COMBO:
			view = new ComboOptionView((ZLComboOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
		case COLOR:
			view = new ColorOptionView((ZLColorOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
		case KEY:
			view = new KeyOptionView((ZLKeyOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
	}

	if (view != 0) {
		view->setVisible(option->isVisible());
		myViews.push_back(view);
	}
}

// vim:ts=2:sw=2:noet
