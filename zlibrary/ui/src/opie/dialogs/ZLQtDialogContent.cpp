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

#include <qapplication.h>
#include <qlayout.h>

#include "ZLQtDialogContent.h"
#include "ZLQtOptionView.h"

void ZLQtDialogContent::close() {
	myLayout->setRowStretch(myRowCounter, 10);
}

ZLQtDialogContent::ZLQtDialogContent(QWidget *parent, const ZLResource &resource) : ZLDialogContent(resource), myParentWidget(parent) {
	myWidget = new QWidget(myParentWidget);
	const long displaySize = qApp->desktop()->height() * (long)qApp->desktop()->width();
	const int space = (displaySize < 640 * 480) ? 3 : 10;
	myLayout = new QGridLayout(myWidget, -1, 13, space, space);
	myRowCounter = 0;
}

ZLQtDialogContent::~ZLQtDialogContent() {
}

void ZLQtDialogContent::addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option) {
	createViewByEntry(name, tooltip, option, 0, 12);
	++myRowCounter;
}

void ZLQtDialogContent::addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0, const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1) {
	createViewByEntry(name0, tooltip0, option0, 0, 5);
	createViewByEntry(name1, tooltip1, option1, 7, 12);
	++myRowCounter;
}

void ZLQtDialogContent::addItem(QWidget *widget, int row, int fromColumn, int toColumn) {
	myLayout->addMultiCellWidget(widget, row, row, fromColumn, toColumn);
}

void ZLQtDialogContent::createViewByEntry(const std::string &name, const std::string &tooltip, ZLOptionEntry *option, int fromColumn, int toColumn) {
	if (option == 0) {
		return;
	}

	ZLQtOptionView *view = 0;
	switch (option->kind()) {
		case ZLOptionEntry::BOOLEAN:
			view = new BooleanOptionView(name, tooltip, (ZLBooleanOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case ZLOptionEntry::BOOLEAN3:
			view = new Boolean3OptionView(name, tooltip, (ZLBoolean3OptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case ZLOptionEntry::STRING:
			view = new StringOptionView(name, tooltip, (ZLStringOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case ZLOptionEntry::MULTILINE:
			view = new MultilineOptionView(name, tooltip, (ZLMultilineOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case ZLOptionEntry::CHOICE:
			view = new ChoiceOptionView(name, tooltip, (ZLChoiceOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case ZLOptionEntry::SPIN:
			view = new SpinOptionView(name, tooltip, (ZLSpinOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case ZLOptionEntry::COMBO:
			view = new ComboOptionView(name, tooltip, (ZLComboOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case ZLOptionEntry::COLOR:
			view = new ColorOptionView(name, tooltip, (ZLColorOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case ZLOptionEntry::KEY:
			view = new KeyOptionView(name, tooltip, (ZLKeyOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case ZLOptionEntry::ORDER:
			// TODO: implement
			break;
	}

	if (view != 0) {
		view->setVisible(option->isVisible());
		addView(view);
	}
}

QWidget *ZLQtDialogContent::widget() {
	return myWidget;
}

QWidget *ZLQtDialogContent::parentWidget() {
	return myParentWidget;
}
