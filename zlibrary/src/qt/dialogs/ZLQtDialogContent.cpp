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

#include <qapplication.h>
#include <qlayout.h>

#include "ZLQtDialogContent.h"
#include "ZLQtOptionView.h"

void ZLQtDialogContent::close() {
	myLayout->setRowStretch(myRowCounter, 10);
}

ZLQtDialogContent::ZLQtDialogContent(QWidget *parent) : QWidget(parent), myParentWidget(parent) {
	const long displaySize = qApp->desktop()->height() * (long)qApp->desktop()->width();
	const int space = (displaySize < 640 * 480) ? 3 : 10;
	myLayout = new QGridLayout(this, -1, 13, space, space);
	myRowCounter = 0;
}

ZLQtDialogContent::~ZLQtDialogContent() {
}

void ZLQtDialogContent::addOption(ZLOptionEntry *option) {
	createViewByEntry(option, 0, 12);
	++myRowCounter;
}

void ZLQtDialogContent::addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1) {
	createViewByEntry(option0, 0, 5);
	createViewByEntry(option1, 7, 12);
	++myRowCounter;
}

void ZLQtDialogContent::addItem(QWidget *widget, int row, int fromColumn, int toColumn) {
	myLayout->addMultiCellWidget(widget, row, row, fromColumn, toColumn);
}

void ZLQtDialogContent::createViewByEntry(ZLOptionEntry *option, int fromColumn, int toColumn) {
	if (option == 0) {
		return;
	}
	ZLQtOptionView *view = 0;
	switch (option->kind()) {
		case BOOLEAN:
			view = new BooleanOptionView((ZLBooleanOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case STRING:
			view = new StringOptionView((ZLStringOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case CHOICE:
			view = new ChoiceOptionView((ZLChoiceOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case SPIN:
			view = new SpinOptionView((ZLSpinOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case COMBO:
			view = new ComboOptionView((ZLComboOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case COLOR:
			view = new ColorOptionView((ZLColorOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case KEY:
			view = new KeyOptionView((ZLKeyOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case ORDER:
			view = new OrderOptionView((ZLOrderOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
	}

	if (view != 0) {
		view->setVisible(option->isVisible());
		addView(view);
	}
}

QWidget *ZLQtDialogContent::parentWidget() {
	return myParentWidget;
}
