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

#include "ZLGtkDialogContent.h"
#include "ZLGtkOptionView.h"

ZLGtkDialogContent::ZLGtkDialogContent(const ZLResource &resource) : ZLDialogContent(resource) {
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

void ZLGtkDialogContent::addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option) {
	int row = addRow();

	createViewByEntry(name, tooltip, option, row, 0, 12);
}

void ZLGtkDialogContent::addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0, const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1) {
	int row = addRow();

	createViewByEntry(name0, tooltip0, option0, row, 0, 6);
	createViewByEntry(name1, tooltip1, option1, row, 6, 12);
}

void ZLGtkDialogContent::createViewByEntry(const std::string &name, const std::string &tooltip, ZLOptionEntry *option, int row, int fromColumn, int toColumn) {
	if (option == 0) {
		return;
	}

	ZLGtkOptionView *view = 0;

	switch (option->kind()) {
		case ZLOptionEntry::BOOLEAN:
			view = new BooleanOptionView(name, tooltip, (ZLBooleanOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
		case ZLOptionEntry::BOOLEAN3:
			view = new Boolean3OptionView(name, tooltip, (ZLBoolean3OptionEntry*)option, this, row, fromColumn, toColumn);
			break;
		case ZLOptionEntry::STRING:
			view = new StringOptionView(name, tooltip, (ZLStringOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
		case ZLOptionEntry::CHOICE:
			view = new ChoiceOptionView(name, tooltip, (ZLChoiceOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
		case ZLOptionEntry::SPIN:
			view = new SpinOptionView(name, tooltip, (ZLSpinOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
		case ZLOptionEntry::COMBO:
			view = new ComboOptionView(name, tooltip, (ZLComboOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
		case ZLOptionEntry::COLOR:
			view = new ColorOptionView(name, tooltip, (ZLColorOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
		case ZLOptionEntry::KEY:
			view = new KeyOptionView(name, tooltip, (ZLKeyOptionEntry*)option, this, row, fromColumn, toColumn);
			break;
	}

	if (view != 0) {
		view->setVisible(option->isVisible());
		addView(view);
	}
}
