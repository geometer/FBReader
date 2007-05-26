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

#include "ZLSimpleOptionEntry.h"

ZLSimpleStringOptionEntry::ZLSimpleStringOptionEntry(ZLStringOption &option) : myOption(option) {
}

const std::string &ZLSimpleStringOptionEntry::initialValue() const {
	return myOption.value();
}

void ZLSimpleStringOptionEntry::onAccept(const std::string &value) {
	myOption.setValue(value);
}

ZLSimpleBooleanOptionEntry::ZLSimpleBooleanOptionEntry(ZLBooleanOption &option) : myOption(option) {
}

bool ZLSimpleBooleanOptionEntry::initialState() const {
	return myOption.value();
}

void ZLSimpleBooleanOptionEntry::onAccept(bool state) {
	myOption.setValue(state);
}

ZLSimpleSpinOptionEntry::ZLSimpleSpinOptionEntry(ZLIntegerRangeOption &option, int step) : myOption(option), myStep(step) {
}

int ZLSimpleSpinOptionEntry::initialValue() const {
	return myOption.value();
}

int ZLSimpleSpinOptionEntry::minValue() const {
	return myOption.minValue();
}

int ZLSimpleSpinOptionEntry::maxValue() const {
	return myOption.maxValue();
}

int ZLSimpleSpinOptionEntry::step() const {
	return myStep;
}

void ZLSimpleSpinOptionEntry::onAccept(int value) {
	myOption.setValue(value);
}

std::vector<std::string> ZLSimpleBoolean3OptionEntry::ourValues;

ZLSimpleBoolean3OptionEntry::ZLSimpleBoolean3OptionEntry(ZLBoolean3Option &option) : myOption(option) {
}

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

void ZLSimpleBoolean3OptionEntry::onAccept(const std::string &value) {
	for (unsigned int i = 0; i < values().size(); ++i) {
		if (values()[i] == value) {
			myOption.setValue((Boolean3)i);
			return;
		}
	}
}

ZLSimpleColorOptionEntry::ZLSimpleColorOptionEntry(ZLColorOption &option) : myOption(option) {
}

const ZLColor ZLSimpleColorOptionEntry::color() const {
	return myOption.value();
}

void ZLSimpleColorOptionEntry::onAccept(ZLColor color) {
	myOption.setValue(color);
}
