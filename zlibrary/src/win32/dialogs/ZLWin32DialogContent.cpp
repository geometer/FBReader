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

#include "ZLWin32DialogContent.h"
//#include "ZLWin32OptionView.h"

ZLWin32DialogContent::ZLWin32DialogContent() {
	myContentBox = new ZLWin32DialogVBox();
	myContentBoxAsElementPtr = myContentBox;
	myContentBox->setHomogeneous(true);

	ZLWin32DialogElementPtr control = new ZLWin32LineEditor(10001, "My Editor");
	control->setVisible(true);
	myContentBox->addElement(control);
	control = new ZLWin32CheckBox(10001, "My Checkbox");
	control->setVisible(true);
	myContentBox->addElement(control);

	/*
	myTable = GTK_TABLE(gtk_table_new(0, 4, false));
	gtk_container_set_border_width(GTK_CONTAINER(myTable), 2);

	myRowCounter = 0;

	gtk_widget_show(GTK_WIDGET(myTable));
	*/
}

ZLWin32DialogContent::~ZLWin32DialogContent() {
	// We must not delete the widget, it's destroyed when the parent widget is destroyed
}

ZLWin32DialogElementPtr ZLWin32DialogContent::content() const {
	return myContentBoxAsElementPtr;
}

int ZLWin32DialogContent::addRow() {
	/*
	int row = myRowCounter++;

	gtk_table_resize(myTable, myRowCounter, 2);

	return row;
	*/
	return 0;
}

//void ZLWin32DialogContent::addItem(Win32Widget *what, int row, int fromColumn, int toColumn) {
	//gtk_table_attach(myTable, what, fromColumn, toColumn, row, row + 1, (Win32AttachOptions)(GTK_FILL | GTK_EXPAND), GTK_FILL, 2, 1);
//}

void ZLWin32DialogContent::addOption(ZLOptionEntry *option) {
	//int row = addRow();

	//createViewByEntry(option, row, 0, 4);
}

void ZLWin32DialogContent::addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1) {
	//int row = addRow();

	//createViewByEntry(option0, row, 0, 2);
	//createViewByEntry(option1, row, 2, 4);
}

void ZLWin32DialogContent::createViewByEntry(ZLOptionEntry *option, int row, ZLWin32OptionView::HorizontalLocation location) {
	if (option == 0) {
		return;
	}

	ZLWin32OptionView *view = 0;

	switch (option->kind()) {
		case ZLOptionEntry::BOOLEAN:
			//view = new BooleanOptionView((ZLBooleanOptionEntry*)option, this, row, location);
			break;
		case ZLOptionEntry::STRING:
			view = new StringOptionView((ZLStringOptionEntry*)option, this, row, location);
			break;
		case ZLOptionEntry::CHOICE:
			//view = new ChoiceOptionView((ZLChoiceOptionEntry*)option, this, row, location);
			break;
		case ZLOptionEntry::SPIN:
			//view = new SpinOptionView((ZLSpinOptionEntry*)option, this, row, location);
			break;
		case ZLOptionEntry::COMBO:
			//view = new ComboOptionView((ZLComboOptionEntry*)option, this, row, location);
			break;
		case ZLOptionEntry::COLOR:
			//view = new ColorOptionView((ZLColorOptionEntry*)option, this, row, location);
			break;
		case ZLOptionEntry::KEY:
			//view = new KeyOptionView((ZLKeyOptionEntry*)option, this, row, location);
			break;
		case ZLOptionEntry::ORDER:
			// TODO: implement
			break;
		case ZLOptionEntry::MULTILINE:
			// TODO: implement
			break;
	}

	if (view != 0) {
		view->setVisible(option->isVisible());
		addView(view);
	}
}
