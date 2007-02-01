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
	myContentBox = new W32VBox();
	myContentBoxAsElementPtr = myContentBox;
	myContentBox->setHomogeneous(true);

	/*
	W32ElementPtr control = new W32LineEditor(10001, "My Editor");
	control->setVisible(true);
	myContentBox->addElement(control);
	control = new W32CheckBox(10001, "My Checkbox");
	control->setVisible(true);
	myContentBox->addElement(control);
	*/
}

ZLWin32DialogContent::~ZLWin32DialogContent() {
}

W32ElementPtr ZLWin32DialogContent::content() const {
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
	ZLWin32OptionView *view = createViewByEntry(option);
	if (view != 0) {
		myContentBox->addElement(view->element());
	}
}

void ZLWin32DialogContent::addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1) {
	ZLWin32OptionView *view0 = createViewByEntry(option0);
	ZLWin32OptionView *view1 = createViewByEntry(option1);
	if ((view0 != 0) || (view1 != 0)) {
		W32HBox *box = new W32HBox();
		box->setHomogeneous(true);
		myContentBox->addElement(box);
		if (view0 != 0) {
			box->addElement(view0->element());
		} else {
			// TODO: add dummy element
		}
		if (view1 != 0) {
			box->addElement(view1->element());
		} else {
			// TODO: add dummy element
		}
	}
}

ZLWin32OptionView *ZLWin32DialogContent::createViewByEntry(ZLOptionEntry *option) {
	if (option == 0) {
		return 0;
	}

	ZLWin32OptionView *view = 0;

	switch (option->kind()) {
		case ZLOptionEntry::BOOLEAN:
			view = new BooleanOptionView((ZLBooleanOptionEntry*)option, this);
			break;
		case ZLOptionEntry::STRING:
			view = new StringOptionView((ZLStringOptionEntry*)option, this);
			break;
		case ZLOptionEntry::CHOICE:
			//view = new ChoiceOptionView((ZLChoiceOptionEntry*)option, this);
			break;
		case ZLOptionEntry::SPIN:
			view = new SpinOptionView((ZLSpinOptionEntry*)option, this);
			break;
		case ZLOptionEntry::COMBO:
			//view = new ComboOptionView((ZLComboOptionEntry*)option, this);
			break;
		case ZLOptionEntry::COLOR:
			//view = new ColorOptionView((ZLColorOptionEntry*)option, this);
			break;
		case ZLOptionEntry::KEY:
			//view = new KeyOptionView((ZLKeyOptionEntry*)option, this);
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
	return view;
}
