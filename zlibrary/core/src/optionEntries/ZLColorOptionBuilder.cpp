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

#include <vector>
#include <map>

#include "ZLColorOptionBuilder.h"

class ZLColorOptionsData {

private:
	ZLComboOptionEntry *myComboEntry;
	ZLColorOptionEntry *myColorEntry;

	std::string myCurrentOptionName;
	std::string myPreviousOptionName;
	std::vector<std::string> myOptionNames;
	std::map<std::string,ZLColor> myCurrentColors;
	std::map<std::string,ZLColorOption*> myOptions;

friend class ZLColorOptionBuilder;
friend class ZLColorComboOptionEntry;
friend class ZLMultiColorOptionEntry;
};

class ZLColorComboOptionEntry : public ZLComboOptionEntry {

public:
	ZLColorComboOptionEntry(shared_ptr<ZLColorOptionsData> data);

private:
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onValueSelected(int index);
	void onAccept(const std::string &value);

private:
	shared_ptr<ZLColorOptionsData> myData;
};

class ZLMultiColorOptionEntry : public ZLColorOptionEntry {

public:
	ZLMultiColorOptionEntry(shared_ptr<ZLColorOptionsData> data);

private:
	const ZLColor initialColor() const;
	const ZLColor color() const;
	void onReset(ZLColor color);
	void onAccept(ZLColor color);

private:
	shared_ptr<ZLColorOptionsData> myData;
};

ZLColorOptionBuilder::ZLColorOptionBuilder() {
	myData = new ZLColorOptionsData();
	myData->myComboEntry = new ZLColorComboOptionEntry(myData);
	myData->myColorEntry = new ZLMultiColorOptionEntry(myData);
}

ZLColorOptionBuilder::~ZLColorOptionBuilder() {
}

void ZLColorOptionBuilder::addOption(const std::string &name, ZLColorOption &option) {
	myData->myOptionNames.push_back(name);
	myData->myCurrentColors[name] = option.value();
	myData->myOptions[name] = &option;
}

void ZLColorOptionBuilder::setInitial(const std::string &name) {
	myData->myCurrentOptionName = name;
	myData->myPreviousOptionName = name;
}

ZLOptionEntry *ZLColorOptionBuilder::comboEntry() const {
	return myData->myComboEntry;
}

ZLOptionEntry *ZLColorOptionBuilder::colorEntry() const {
	return myData->myColorEntry;
}

ZLColorComboOptionEntry::ZLColorComboOptionEntry(shared_ptr<ZLColorOptionsData> data) : myData(data) {
}

const std::string &ZLColorComboOptionEntry::initialValue() const {
	return myData->myCurrentOptionName;
}

const std::vector<std::string> &ZLColorComboOptionEntry::values() const {
	return myData->myOptionNames;
}

void ZLColorComboOptionEntry::onValueSelected(int index) {
	myData->myCurrentOptionName = values()[index];
	myData->myColorEntry->resetView();
	myData->myPreviousOptionName = myData->myCurrentOptionName;
}

void ZLColorComboOptionEntry::onAccept(const std::string&) {
}

ZLMultiColorOptionEntry::ZLMultiColorOptionEntry(shared_ptr<ZLColorOptionsData> data) : myData(data) {
}

const ZLColor ZLMultiColorOptionEntry::initialColor() const {
	return myData->myOptions[myData->myCurrentOptionName]->value();
}

const ZLColor ZLMultiColorOptionEntry::color() const {
	ZLColor color = myData->myCurrentColors[myData->myCurrentOptionName];
	return myData->myCurrentColors[myData->myCurrentOptionName];
}

void ZLMultiColorOptionEntry::onReset(ZLColor color) {
	myData->myCurrentColors[myData->myPreviousOptionName] = color;
}

void ZLMultiColorOptionEntry::onAccept(ZLColor color) {
	onReset(color);
	for (std::vector<std::string>::const_iterator it = myData->myOptionNames.begin(); it != myData->myOptionNames.end(); ++it) {
		myData->myOptions[*it]->setValue(myData->myCurrentColors[*it]);
	}
}
