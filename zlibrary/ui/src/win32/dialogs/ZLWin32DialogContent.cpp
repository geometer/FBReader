/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

ZLWin32DialogContent::ZLWin32DialogContent(const ZLResource &resource) : ZLDialogContent(resource), myRowCounter(0) {
	myContentTable = new W32Table();
}

ZLWin32DialogContent::~ZLWin32DialogContent() {
}

W32WidgetPtr ZLWin32DialogContent::contentPtr() const {
	return myContentTable;
}

W32Table &ZLWin32DialogContent::contentTable() const {
	return (W32Table&)*myContentTable;
}

void ZLWin32DialogContent::addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option) {
	createViewByEntry(name, tooltip, option, 0, 11);
	++myRowCounter;
}

void ZLWin32DialogContent::addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0, const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1) {
	createViewByEntry(name0, tooltip0, option0, 0, 5);
	createViewByEntry(name1, tooltip1, option1, 6, 11);
	++myRowCounter;
}

void ZLWin32DialogContent::insertWidget(W32WidgetPtr widget, int from, int to) {
	contentTable().setElement(widget, myRowCounter, from, to);
}

void ZLWin32DialogContent::createViewByEntry(const std::string &name, const std::string &tooltip, ZLOptionEntry *option, int from, int to) {
	if (option == 0) {
		return;
	}

	ZLWin32OptionView *view = 0;

	switch (option->kind()) {
		case ZLOptionEntry::BOOLEAN:
			view = new BooleanOptionView(name, tooltip, (ZLBooleanOptionEntry*)option, *this, from, to);
			break;
		case ZLOptionEntry::BOOLEAN3:
			view = new Boolean3OptionView(name, tooltip, (ZLBoolean3OptionEntry*)option, *this, from, to);
			break;
		case ZLOptionEntry::STRING:
			view = new StringOptionView(name, tooltip, (ZLStringOptionEntry*)option, *this, from, to);
			break;
		case ZLOptionEntry::CHOICE:
			view = new ChoiceOptionView(name, tooltip, (ZLChoiceOptionEntry*)option, *this, from, to);
			break;
		case ZLOptionEntry::SPIN:
			view = new SpinOptionView(name, tooltip, (ZLSpinOptionEntry*)option, *this, from, to);
			break;
		case ZLOptionEntry::COMBO:
			view = new ComboOptionView(name, tooltip, (ZLComboOptionEntry*)option, *this, from, to);
			break;
		case ZLOptionEntry::COLOR:
			view = new ColorOptionView(name, tooltip, (ZLColorOptionEntry*)option, *this, from, to);
			break;
		case ZLOptionEntry::KEY:
			view = new KeyOptionView(name, tooltip, (ZLKeyOptionEntry*)option, *this, from, to);
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
