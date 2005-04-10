/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "ZLOptionEntry.h"

OptionView::OptionView(ZLOptionEntry *option) : myOption(option) {
	myOption->setView(this);
}

OptionView::~OptionView() {
	delete myOption;
}

void ZLOptionEntry::setVisible(bool visible) {
	myIsVisible = visible;
	if (myView != 0) {
		myView->setVisible(visible);
	}
}

void ZLOptionEntry::setActive(bool active) {
	myIsActive = active;
	if (myView != 0) {
		myView->setActive(active);
	}
}

std::vector<std::string> ZLSimpleBoolean3OptionEntry::ourValues;

const std::string &ZLSimpleBoolean3OptionEntry::initialValue() const {
	return values()[myOption.value()];
}

const std::vector<std::string> &ZLSimpleBoolean3OptionEntry::values() const {
	if (ourValues.empty()) {
		ourValues.push_back("No");
		ourValues.push_back("Yes");
		ourValues.push_back("<base>");
	}
	return ourValues;
}

void ZLSimpleBoolean3OptionEntry::onAccept(const std::string &value) const {
	for (unsigned int i = 0; i < values().size(); i++) {
		if (values()[i] == value) {
			myOption.setValue((Boolean3)i);
			return;
		}
	}
}

static std::string EMPTY_STRING;

const std::string &ZLSimpleColorOptionEntry::name() const {
	return EMPTY_STRING;
}
