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

#include "ZLGtkDialogContent.h"
#include "ZLGtkOptionView.h"

ZLGtkDialogContent::ZLGtkDialogContent() {
	myRowCounter = 0;
	myTable = GTK_TABLE(gtk_table_new(0, 12, false));
	gtk_widget_show_all(GTK_WIDGET(myTable));
}

ZLGtkDialogContent::~ZLGtkDialogContent() {
	// We must not delete the widget, it's destroyed when the parent widget is destroyed
}

int ZLGtkDialogContent::addRow() {
	int row = myRowCounter++;

	gtk_table_resize(myTable, myRowCounter, 2);

	return row;
}

void ZLGtkDialogContent::addItem(GtkWidget *what, int row, int fromColumn, int toColumn) {
	gtk_table_attach(myTable, what, fromColumn, toColumn, row, row + 1, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_FILL, 2, 1);
}

void ZLGtkDialogContent::addOption(ZLOptionEntry *option) {
	int row = addRow();

	createViewByEntry(option, row, 0, 12);
}

void ZLGtkDialogContent::addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1) {
	int row = addRow();

	createViewByEntry(option0, row, 0, 6);
	createViewByEntry(option1, row, 6, 12);
}

void ZLGtkDialogContent::createViewByEntry(ZLOptionEntry *option, int row, int fromColumn, int toColumn) {
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
		addView(view);
	}
}
