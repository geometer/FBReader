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

#include <algorithm>
#include <iostream>

#include <gtk/gtkstock.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkbox.h>

#include "../../abstract/dialogs/ZLOptionEntry.h"

#include "GtkOptionsDialog.h"
#include "GtkOptionView.h"

GtkOptionsDialog::GtkOptionsDialog(const char *caption) {
  myDialog = GTK_DIALOG(gtk_dialog_new_with_buttons (caption, NULL, GTK_DIALOG_MODAL,
          GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
          GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
          NULL));

  myNotebook = GTK_NOTEBOOK(gtk_notebook_new ());

  gtk_container_set_border_width (GTK_CONTAINER (myNotebook), 8);
  gtk_box_pack_start (GTK_BOX (myDialog->vbox), GTK_WIDGET(myNotebook), TRUE, TRUE, 0);

  gtk_widget_show (GTK_WIDGET(myNotebook));
}

GtkOptionsDialog::~GtkOptionsDialog() {
  // I do not have to destroy myNotebook as it's a myDialog child
  for (std::vector<GtkOptionsDialogTab *>::iterator tab = myTabs.begin (); tab != myTabs.end (); ++tab)
    delete *tab;

  gtk_widget_destroy (GTK_WIDGET(myDialog));
}

ZLOptionsDialogTab *GtkOptionsDialog::createTab(const std::string &name) {
  GtkOptionsDialogTab *tab = new GtkOptionsDialogTab();
  GtkWidget *label = gtk_label_new(name.c_str());

  gtk_notebook_append_page (myNotebook, tab->widget(), label);

  myTabs.push_back(tab);
  myTabNames.push_back(name);

  return tab;
}

std::string GtkOptionsDialog::selectedTabName() {
  return myTabNames[gtk_notebook_get_current_page(myNotebook)];
}

void GtkOptionsDialog::selectTab(const std::string &name) {
  std::vector<std::string>::const_iterator it = std::find(myTabNames.begin(), myTabNames.end(), name);
  if (it != myTabNames.end()) {
    gtk_notebook_set_current_page (myNotebook, it - myTabNames.begin());
  }
}

int GtkOptionsDialog::run() {
  gint response;

  response = gtk_dialog_run (myDialog);

  switch (response) {
    case GTK_RESPONSE_ACCEPT:
      for (std::vector<GtkOptionsDialogTab *>::iterator tab = myTabs.begin (); tab != myTabs.end (); ++tab)
        (*tab)->accept();
      break;
    case GTK_RESPONSE_REJECT:
      break;
  }

  gtk_widget_hide (GTK_WIDGET(myDialog));

  return 0;
}

void GtkOptionsDialogTab::accept() {
	for (std::vector<GtkOptionView *>::iterator view = myViews.begin(); view != myViews.end(); ++view) {
		(*view)->onAccept();
	}
}

GtkOptionsDialogTab::GtkOptionsDialogTab() {
  myTable = GTK_TABLE(gtk_table_new (0, 2, TRUE));
  myRowCounter = 0;

  gtk_widget_show (GTK_WIDGET(myTable));
}

GtkOptionsDialogTab::~GtkOptionsDialogTab() {
  // We must not delete the widget, it's destroyed when the parent widget is destroyed
	for (std::vector<GtkOptionView *>::iterator view = myViews.begin(); view != myViews.end(); ++view) {
		delete (*view);
	}
}

int GtkOptionsDialogTab::addRow (void) {
	int row = myRowCounter++;

	gtk_table_resize (myTable, myRowCounter, 2);

	return row;
}

void GtkOptionsDialogTab::addItem (GtkWidget *what, int row, int fromColumn, int toColumn) {
	gtk_table_attach (myTable, what, fromColumn, toColumn, row, row + 1, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_FILL, 0, 0);
}

void GtkOptionsDialogTab::addOption(ZLOptionEntry *option) {
	int row = addRow ();

	GtkOptionView *optionView = viewByEntry (option, row, 0, 2);

	if (optionView != NULL) {
		optionView->show();
		myViews.push_back (optionView);
	}
}

void GtkOptionsDialogTab::addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1) {
	int row = addRow ();

	GtkOptionView *optionView0 = viewByEntry (option0, row, 0, 1),
							 	*optionView1 = viewByEntry (option1, row, 1, 2);

	if (optionView0 != NULL) {
		optionView0->show();
		myViews.push_back (optionView0);
	}

	if (optionView1 != NULL) {
		optionView1->show();
		myViews.push_back (optionView1);
	}
}

GtkOptionView *GtkOptionsDialogTab::viewByEntry(ZLOptionEntry *option, int row, int fromColumn, int toColumn) {
	if (option == NULL) {
		return NULL;
	}

	switch (option->kind()) {
		default:
			return NULL;
		case BOOLEAN:
			return new BooleanOptionView((ZLBooleanOptionEntry*)option, this, row, fromColumn, toColumn);
		case STRING:
			return new StringOptionView((ZLStringOptionEntry*)option, this, row, fromColumn, toColumn);
#if 0
		case CHOICE:
			return new ChoiceOptionView((ZLChoiceOptionEntry*)option, this, row, fromColumn, toColumn);
#endif
		case SPIN:
			return new SpinOptionView((ZLSpinOptionEntry*)option, this, row, fromColumn, toColumn);
		case COMBO:
			return new ComboOptionView((ZLComboOptionEntry*)option, this, row, fromColumn, toColumn);
		case COLOR:
			return new ColorOptionView((ZLColorOptionEntry*)option, this, row, fromColumn, toColumn);
		case UNKNOWN:
			GtkOptionView* view = (GtkOptionView*)((ZLUserDefinedOptionEntry*)option)->createView();
			view->setPosition(this, row, fromColumn, toColumn);
			if (option->isVisible()) {
				view->createItem();
			}
			return view;
	}
	return NULL;
}

// vim:ts=2:sw=2:noet
